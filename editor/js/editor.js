// ============================================
// Wisam Code Editor - Main Editor
// محرر وسام - المحرر الرئيسي
// ============================================

// Global variables
let currentTheme = 'dark';
let currentFontSize = 16;
let currentFile = 'main.wsm';
let files = {
    'main.wsm': ''
};

// DOM Elements
const codeEditor = document.getElementById('code-editor');
const lineNumbers = document.getElementById('line-numbers');
const outputArea = document.getElementById('output-area');
const tokensArea = document.getElementById('tokens-area');
const astArea = document.getElementById('ast-area');
const charCount = document.getElementById('char-count');
const lineCount = document.getElementById('line-count');
const execTime = document.getElementById('exec-time');
const statusText = document.getElementById('status-text');
const statusIcon = document.getElementById('status-icon');
const currentFileName = document.getElementById('current-file');
const fileList = document.getElementById('file-list');

// Initialize editor
document.addEventListener('DOMContentLoaded', () => {
    initializeEditor();
    loadExample('hello');
    updateLineNumbers();
    updateStatusBar();
});

function initializeEditor() {
    // Set up event listeners
    codeEditor.addEventListener('input', () => {
        updateLineNumbers();
        updateStatusBar();
        saveCurrentFile();
    });
    
    codeEditor.addEventListener('scroll', syncScroll);
    codeEditor.addEventListener('keydown', handleKeyDown);
    
    // Keyboard shortcuts
    document.addEventListener('keydown', (e) => {
        if (e.ctrlKey || e.metaKey) {
            switch (e.key) {
                case 'Enter':
                    e.preventDefault();
                    runCode();
                    break;
                case 's':
                    e.preventDefault();
                    downloadCode();
                    break;
                case ' ':
                    e.preventDefault();
                    showAutocomplete();
                    break;
            }
        }
    });
}

// Update line numbers
function updateLineNumbers() {
    const lines = codeEditor.value.split('\n').length;
    lineNumbers.innerHTML = Array.from({ length: lines }, (_, i) => i + 1).join('<br>');
}

// Sync scroll between editor and line numbers
function syncScroll() {
    lineNumbers.scrollTop = codeEditor.scrollTop;
}

// Handle keydown events
function handleKeyDown(e) {
    if (e.key === 'Tab') {
        e.preventDefault();
        insertText('    ');
    } else if (e.key === 'Enter') {
        handleEnter(e);
    }
}

// Handle Enter key for auto-indentation
function handleEnter(e) {
    const start = codeEditor.selectionStart;
    const end = codeEditor.selectionEnd;
    const value = codeEditor.value;
    
    // Get current line
    const lineStart = value.lastIndexOf('\n', start - 1) + 1;
    const currentLine = value.substring(lineStart, start);
    
    // Calculate indentation
    const indentMatch = currentLine.match(/^[\t ]*/);
    let indent = indentMatch ? indentMatch[0] : '';
    
    // Add extra indent if line ends with certain keywords
    if (/\b(إذا|اذا|لكل|طالما|دالة|هيكل|صنف)\s*$/.test(currentLine)) {
        indent += '    ';
    }
    
    e.preventDefault();
    insertText('\n' + indent);
}

// Insert text at cursor position
function insertText(text) {
    const start = codeEditor.selectionStart;
    const end = codeEditor.selectionEnd;
    const value = codeEditor.value;
    
    codeEditor.value = value.substring(0, start) + text + value.substring(end);
    codeEditor.selectionStart = codeEditor.selectionEnd = start + text.length;
    codeEditor.focus();
    
    updateLineNumbers();
    updateStatusBar();
}

// Update status bar
function updateStatusBar() {
    const text = codeEditor.value;
    const chars = text.length;
    const lines = text.split('\n').length;
    
    charCount.textContent = `${chars} حرف`;
    lineCount.textContent = `${lines} سطر`;
}

// Save current file content
function saveCurrentFile() {
    files[currentFile] = codeEditor.value;
}

// Run code
function runCode() {
    const code = codeEditor.value.trim();
    
    if (!code) {
        showOutput('⚠️ لا يوجد كود للتشغيل', 'warning');
        return;
    }
    
    showLoading(true);
    setStatus('running', 'جاري التشغيل...');
    
    const startTime = performance.now();
    
    try {
        // Tokenize
        const lexer = new Lexer(code);
        const tokens = lexer.tokenize();
        
        // Parse
        const parser = new Parser(tokens);
        const ast = parser.parse();
        
        // Check for parse errors
        if (parser.errors.length > 0) {
            const errorMsg = parser.errors.map(e => `❌ خطأ عند السطر ${e.line}: ${e.message}`).join('\n');
            showOutput(errorMsg, 'error');
            setStatus('error', 'خطأ في التحليل');
            showLoading(false);
            return;
        }
        
        // Execute
        const interpreter = new Interpreter();
        const result = interpreter.run(ast);
        
        const endTime = performance.now();
        const execTimeMs = (endTime - startTime).toFixed(2);
        
        if (result.success) {
            const output = result.output.join('\n');
            showOutput(output || '✓ تم التنفيذ بنجاح (لا يوجد ناتج)', 'success');
            setStatus('success', `تم التنفيذ (${execTimeMs}ms)`);
        } else {
            const errorMsg = result.errors.map(e => `❌ ${e}`).join('\n');
            showOutput(errorMsg, 'error');
            setStatus('error', 'خطأ في التنفيذ');
        }
        
        execTime.textContent = `${execTimeMs}ms`;
        
    } catch (error) {
        showOutput(`❌ خطأ غير متوقع: ${error.message}`, 'error');
        setStatus('error', 'خطأ');
    }
    
    showLoading(false);
}

// Stop code execution (for async operations)
function stopCode() {
    showLoading(false);
    setStatus('ready', 'تم الإيقاف');
}

// Clear output
function clearOutput() {
    outputArea.innerHTML = `
        <div class="welcome-message">
            <i class="fas fa-hand-sparkles"></i>
            <h3>جاهز للتشغيل!</h3>
            <p>اكتب الكود واضغط تشغيل</p>
        </div>
    `;
    tokensArea.innerHTML = '';
    astArea.innerHTML = '';
    execTime.textContent = '--';
}

// Show output
function showOutput(text, type = 'normal') {
    const lines = text.split('\n');
    const formattedLines = lines.map(line => {
        let className = 'output-line';
        if (type === 'error' || line.startsWith('❌')) className += ' output-error';
        if (type === 'success' || line.startsWith('✓')) className += ' output-success';
        if (type === 'warning' || line.startsWith('⚠️')) className += ' output-warning';
        return `<div class="${className}">${escapeHtml(line)}</div>`;
    }).join('');
    
    outputArea.innerHTML = formattedLines;
}

// Escape HTML
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Format code
function formatCode() {
    const code = codeEditor.value;
    // Simple formatting - add proper indentation
    const lines = code.split('\n');
    let indentLevel = 0;
    const formattedLines = lines.map(line => {
        const trimmed = line.trim();
        
        // Decrease indent for end keywords
        if (/^(انتهى|وإلا)$/.test(trimmed)) {
            indentLevel = Math.max(0, indentLevel - 1);
        }
        
        const formatted = '    '.repeat(indentLevel) + trimmed;
        
        // Increase indent for block-starting keywords
        if (/^(إذا|اذا|لكل|طالما|دالة|هيكل|صنف|إذن|اذن)$/.test(trimmed)) {
            indentLevel++;
        }
        
        return formatted;
    });
    
    codeEditor.value = formattedLines.join('\n');
    updateLineNumbers();
    saveCurrentFile();
    showToast('تم تنسيق الكود');
}

// Download code
function downloadCode() {
    const code = codeEditor.value;
    const blob = new Blob([code], { type: 'text/plain;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    
    const a = document.createElement('a');
    a.href = url;
    a.download = currentFile;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    
    URL.revokeObjectURL(url);
    showToast('تم تحميل الملف');
}

// Share code
function shareCode() {
    const code = codeEditor.value;
    const encoded = btoa(encodeURIComponent(code));
    const shareUrl = `${window.location.origin}${window.location.pathname}?code=${encoded}`;
    
    document.getElementById('share-link').value = shareUrl;
    showModal('share-modal');
}

// Copy share link
function copyShareLink() {
    const linkInput = document.getElementById('share-link');
    linkInput.select();
    document.execCommand('copy');
    showToast('تم نسخ الرابط');
}

// Share to social media
function shareToTwitter() {
    const code = codeEditor.value.substring(0, 100);
    const text = `أكتب الكود بالعربية مع لغة وسام!\n\n${code}...`;
    const url = encodeURIComponent(window.location.href);
    window.open(`https://twitter.com/intent/tweet?text=${encodeURIComponent(text)}&url=${url}`, '_blank');
}

function shareToFacebook() {
    const url = encodeURIComponent(window.location.href);
    window.open(`https://www.facebook.com/sharer/sharer.php?u=${url}`, '_blank');
}

function shareToWhatsApp() {
    const text = `تفقد لغة وسام - لغة برمجة عربية! ${window.location.href}`;
    window.open(`https://wa.me/?text=${encodeURIComponent(text)}`, '_blank');
}

// Load example
function loadExample(exampleName) {
    const example = EXAMPLES[exampleName];
    if (example) {
        codeEditor.value = example.code;
        updateLineNumbers();
        updateStatusBar();
        saveCurrentFile();
        hideModal('examples-modal');
        showToast(`تم تحميل: ${example.name}`);
        
        // Clear output
        clearOutput();
    }
}

// New file
function newFile() {
    const name = prompt('اسم الملف الجديد:', 'new_file.wsm');
    if (name) {
        if (files[name]) {
            alert('الملف موجود بالفعل!');
            return;
        }
        
        files[name] = '';
        currentFile = name;
        codeEditor.value = '';
        updateFileList();
        updateLineNumbers();
        updateStatusBar();
        showToast('تم إنشاء ملف جديد');
    }
}

// Update file list
function updateFileList() {
    fileList.innerHTML = Object.keys(files).map(file => `
        <div class="file-item ${file === currentFile ? 'active' : ''}" data-file="${file}" onclick="switchFile('${file}')">
            <i class="fas fa-file-code"></i>
            <span>${file}</span>
        </div>
    `).join('');
    
    currentFileName.textContent = currentFile;
}

// Switch file
function switchFile(fileName) {
    saveCurrentFile();
    currentFile = fileName;
    codeEditor.value = files[fileName] || '';
    updateFileList();
    updateLineNumbers();
    updateStatusBar();
}

// Change theme
function changeTheme() {
    const theme = document.getElementById('theme-select').value;
    document.body.setAttribute('data-theme', theme);
    currentTheme = theme;
}

// Change font size
function changeFontSize() {
    const size = document.getElementById('font-size').value;
    codeEditor.style.fontSize = `${size}px`;
    lineNumbers.style.fontSize = `${size}px`;
    currentFontSize = parseInt(size);
}

// Switch output tab
function switchTab(tab) {
    // Update tab buttons
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    event.target.classList.add('active');
    
    // Update content areas
    document.querySelectorAll('#output-content > div').forEach(area => {
        area.classList.add('hidden');
    });
    
    if (tab === 'output') {
        outputArea.classList.remove('hidden');
    } else if (tab === 'tokens') {
        tokensArea.classList.remove('hidden');
        showTokens();
    } else if (tab === 'ast') {
        astArea.classList.remove('hidden');
        showAST();
    }
}

// Show tokens
function showTokens() {
    const code = codeEditor.value.trim();
    if (!code) {
        tokensArea.innerHTML = '<div class="output-line output-warning">لا يوجد كود لتحليله</div>';
        return;
    }
    
    try {
        const lexer = new Lexer(code);
        const tokens = lexer.tokenize();
        
        const tokenNames = {
            'LET': 'ليكن', 'CONST': 'ثابت', 'IF': 'إذا', 'THEN': 'إذن', 'ELSE': 'وإلا',
            'END': 'انتهى', 'FOR': 'لكل', 'FROM': 'من', 'TO': 'إلى', 'FUNCTION': 'دالة',
            'RETURN': 'أعد', 'STRUCT': 'هيكل', 'CLASS': 'صنف', 'IMPORT': 'استورد',
            'CREATE': 'أنشئ', 'AS': 'باسم', 'WHILE': 'طالما', 'BREAK': 'توقف',
            'CONTINUE': 'استمر', 'AND': 'و', 'OR': 'أو', 'NOT': 'ليس', 'IN': 'في',
            'TRY': 'حاول', 'CATCH': 'امسك', 'FINALLY': 'أخيراً', 'THROW': 'ألقِ',
            'SWITCH': 'حسب', 'CASE': 'حالة', 'DEFAULT': 'افتراضي', 'DO': 'نفذ',
            'UNTIL': 'حتى', 'FOREACH': 'لكل_عنصر', 'YIELD': 'أنتج', 'ASYNC': 'غير_متزامن',
            'AWAIT': 'انتظر', 'PUBLIC': 'عام', 'PRIVATE': 'خاص', 'PROTECTED': 'محمي',
            'STATIC': 'ثابت_الصنف', 'EXTENDS': 'يرث', 'IMPLEMENTS': 'ينفذ', 'NEW': 'جديد',
            'THIS': 'هذا', 'SUPER': 'أب', 'NULL': 'فارغ', 'TRUE': 'صحيح', 'FALSE': 'خطأ',
            'NUMBER': 'رقم', 'STRING': 'نص', 'BOOLEAN': 'منطقي', 'ARRAY': 'مصفوفة',
            'OBJECT': 'كائن', 'IDENTIFIER': 'معرف', 'ASSIGN': '=', 'PLUS': '+',
            'MINUS': '-', 'MULTIPLY': '*', 'DIVIDE': '/', 'MODULO': '%', 'POWER': '^',
            'EQUAL': '==', 'NOT_EQUAL': '!=', 'GREATER': '>', 'LESS': '<',
            'GREATER_EQ': '>=', 'LESS_EQ': '<=', 'LPAREN': '(', 'RPAREN': ')',
            'LBRACE': '{', 'RBRACE': '}', 'LBRACKET': '[', 'RBRACKET': ']',
            'COMMA': '،', 'DOT': '.', 'COLON': ':', 'SEMICOLON': '؛',
            'NEWLINE': 'سطر_جديد', 'EOF': 'نهاية', 'PRINT': 'اكتب', 'INPUT': 'ادخل',
            'TYPEOF': 'نوع', 'SIZEOF': 'حجم'
        };
        
        const table = `
            <table class="tokens-table">
                <thead>
                    <tr>
                        <th>السطر</th>
                        <th>العمود</th>
                        <th>النوع</th>
                        <th>القيمة</th>
                    </tr>
                </thead>
                <tbody>
                    ${tokens.filter(t => t.type !== 'EOF').map(t => `
                        <tr>
                            <td>${t.line}</td>
                            <td>${t.column}</td>
                            <td><span class="token-type">${tokenNames[t.type] || t.type}</span></td>
                            <td><code>${escapeHtml(t.value)}</code></td>
                        </tr>
                    `).join('')}
                </tbody>
            </table>
        `;
        
        tokensArea.innerHTML = table;
    } catch (error) {
        tokensArea.innerHTML = `<div class="output-line output-error">خطأ: ${error.message}</div>`;
    }
}

// Show AST
function showAST() {
    const code = codeEditor.value.trim();
    if (!code) {
        astArea.innerHTML = '<div class="output-line output-warning">لا يوجد كود لتحليله</div>';
        return;
    }
    
    try {
        const lexer = new Lexer(code);
        const tokens = lexer.tokenize();
        const parser = new Parser(tokens);
        const ast = parser.parse();
        
        const formatted = Parser.formatAST(ast);
        astArea.innerHTML = `<pre class="ast-output">${escapeHtml(formatted)}</pre>`;
    } catch (error) {
        astArea.innerHTML = `<div class="output-line output-error">خطأ: ${error.message}</div>`;
    }
}

// Modal functions
function showModal(modalId) {
    document.getElementById(modalId).classList.add('active');
}

function hideModal(modalId) {
    document.getElementById(modalId).classList.remove('active');
}

// Close modals on outside click
document.addEventListener('click', (e) => {
    if (e.target.classList.contains('modal')) {
        e.target.classList.remove('active');
    }
});

// Toast notification
function showToast(message) {
    const toast = document.getElementById('toast');
    const toastMessage = document.getElementById('toast-message');
    
    toastMessage.textContent = message;
    toast.classList.add('show');
    
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

// Loading overlay
function showLoading(show) {
    const loading = document.getElementById('loading-overlay');
    if (show) {
        loading.classList.add('active');
    } else {
        loading.classList.remove('active');
    }
}

// Set status
function setStatus(status, message) {
    statusText.textContent = message;
    
    const icons = {
        ready: 'fa-circle-notch',
        running: 'fa-spinner fa-spin',
        success: 'fa-check-circle',
        error: 'fa-exclamation-circle'
    };
    
    statusIcon.className = `fas ${icons[status] || icons.ready}`;
}

// Autocomplete (basic)
function showAutocomplete() {
    const keywords = [
        'ليكن', 'ثابت', 'إذا', 'وإلا', 'انتهى', 'لكل', 'من', 'إلى',
        'دالة', 'أعد', 'هيكل', 'صنف', 'استورد', 'أنشئ', 'باسم',
        'طالما', 'توقف', 'استمر', 'و', 'أو', 'ليس', 'في',
        'اكتب', 'ادخل', 'صحيح', 'خطأ', 'فارغ'
    ];
    
    // Simple autocomplete - could be expanded
    showToast('الإكمال التلقائي: اكتب بداية الكلمة واضغط Ctrl+Space');
}

// Load code from URL parameter
function loadCodeFromURL() {
    const urlParams = new URLSearchParams(window.location.search);
    const encodedCode = urlParams.get('code');
    
    if (encodedCode) {
        try {
            const code = decodeURIComponent(atob(encodedCode));
            codeEditor.value = code;
            updateLineNumbers();
            updateStatusBar();
            saveCurrentFile();
            showToast('تم تحميل الكود من الرابط');
        } catch (e) {
            console.error('Failed to load code from URL:', e);
        }
    }
}

// Initialize with URL code if present
window.addEventListener('load', loadCodeFromURL);
