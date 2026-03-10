import { useState, useRef, useCallback } from 'react';
import Editor from '@monaco-editor/react';
import { 
  Play, 
  Save, 
  FolderOpen, 
  FilePlus, 
  Settings, 
  Moon, 
  Sun,
  Code,
  Terminal,
  ChevronRight,
  Folder,
  FileText,
  X
} from 'lucide-react';
import { Button } from '@/components/ui/button';
import { 
  ResizableHandle, 
  ResizablePanel, 
  ResizablePanelGroup 
} from '@/components/ui/resizable';
import { ScrollArea } from '@/components/ui/scroll-area';
import { Separator } from '@/components/ui/separator';
import { 
  Dialog, 
  DialogContent, 
  DialogHeader, 
  DialogTitle,
  DialogTrigger
} from '@/components/ui/dialog';
import { Input } from '@/components/ui/input';
import { toast } from 'sonner';
import './App.css';

// Wisam language keywords and snippets
const WISAM_KEYWORDS = [
  'دالة', 'إرجاع', 'إذا', 'وإلا', 'طالما', 'لكل', 'في', 'صحيح', 'خطأ', 'عدم',
  'طباعة', 'إدخال', 'طول', 'نوع', 'جديد', 'حذف', 'توقف', 'استمرار',
  'function', 'return', 'if', 'else', 'while', 'for', 'in', 'true', 'false', 'null',
  'print', 'input', 'len', 'typeof', 'new', 'delete', 'break', 'continue'
];

const WISAM_SNIPPETS = [
  { label: 'دالة', insertText: 'دالة ${1:name}(${2:params}) {\n\t$0\n}' },
  { label: 'function', insertText: 'function ${1:name}(${2:params}) {\n\t$0\n}' },
  { label: 'إذا', insertText: 'إذا (${1:condition}) {\n\t$0\n}' },
  { label: 'if', insertText: 'if (${1:condition}) {\n\t$0\n}' },
  { label: 'طالما', insertText: 'طالما (${1:condition}) {\n\t$0\n}' },
  { label: 'while', insertText: 'while (${1:condition}) {\n\t$0\n}' },
  { label: 'لكل', insertText: 'لكل (${1:item} في ${2:list}) {\n\t$0\n}' },
  { label: 'for', insertText: 'for (${1:item} in ${2:list}) {\n\t$0\n}' },
];

interface FileItem {
  id: string;
  name: string;
  content: string;
  isOpen: boolean;
}

interface FolderItem {
  id: string;
  name: string;
  files: FileItem[];
  isOpen: boolean;
}

export default function App() {
  const [darkMode, setDarkMode] = useState(true);
  const [code, setCode] = useState<string>(`# مرحباً بك في محرر وسام
# Welcome to Wisam Editor

دالة تحية(اسم) {
    طباعة("مرحباً، " + اسم + "!")
}

دالة رئيسية() {
    اسم = إدخال("ما اسمك؟ ")
    تحية(اسم)
    
    # حساب المجموع
    مجموع = 0
    لكل (رقم في [1, 2, 3, 4, 5]) {
        مجموع = مجموع + رقم
    }
    طباعة("المجموع: " + مجموع)
}

رئيسية()
`);
  const [output, setOutput] = useState<string>('');
  const [consoleOpen, setConsoleOpen] = useState(true);
  const [activeFile, setActiveFile] = useState<string>('main.wsm');
  const [folders, setFolders] = useState<FolderItem[]>([
    {
      id: '1',
      name: 'مشروعي',
      isOpen: true,
      files: [
        { id: '1-1', name: 'main.wsm', content: '# الملف الرئيسي', isOpen: true },
        { id: '1-2', name: 'utils.wsm', content: '# دوال مساعدة', isOpen: false },
      ]
    }
  ]);
  const editorRef = useRef<any>(null);

  // Configure Monaco Editor for Wisam language
  const handleEditorWillMount = useCallback((monaco: any) => {
    // Register Wisam language
    monaco.languages.register({ id: 'wisam' });
    
    // Set language configuration
    monaco.languages.setLanguageConfiguration('wisam', {
      comments: {
        lineComment: '#',
        blockComment: ['/*', '*/']
      },
      brackets: [
        ['{', '}'],
        ['[', ']'],
        ['(', ')']
      ],
      autoClosingPairs: [
        { open: '{', close: '}' },
        { open: '[', close: ']' },
        { open: '(', close: ')' },
        { open: '"', close: '"' },
        { open: "'", close: "'" }
      ],
      surroundingPairs: [
        { open: '{', close: '}' },
        { open: '[', close: ']' },
        { open: '(', close: ')' },
        { open: '"', close: '"' },
        { open: "'", close: "'" }
      ]
    });

    // Define syntax highlighting
    monaco.languages.setMonarchTokensProvider('wisam', {
      keywords: WISAM_KEYWORDS,
      operators: ['+', '-', '*', '/', '%', '=', '==', '!=', '<', '>', '<=', '>=', '&&', '||', '!'],
      symbols: /[=><!~?:&|+\-*\/\^%]+/,
      tokenizer: {
        root: [
          [/[a-zA-Z_\u0600-\u06FF][a-zA-Z0-9_\u0600-\u06FF]*/, { 
            cases: { 
              '@keywords': 'keyword',
              '@default': 'identifier' 
            } 
          }],
          [/[0-9]+/, 'number'],
          [/"[^"]*"/, 'string'],
          [/'[^']*'/, 'string'],
          [/#.*$/, 'comment'],
          [/[\{\}\[\]\(\)]/, '@brackets'],
          [/@symbols/, { cases: { '@operators': 'operator', '@default': '' } }],
          [/\s+/, 'white'],
        ]
      }
    });

    // Define theme
    monaco.editor.defineTheme('wisam-dark', {
      base: 'vs-dark',
      inherit: true,
      rules: [
        { token: 'keyword', foreground: 'FF7B72', fontStyle: 'bold' },
        { token: 'identifier', foreground: 'DCDCAA' },
        { token: 'number', foreground: '79C0FF' },
        { token: 'string', foreground: 'A5D6FF' },
        { token: 'comment', foreground: '8B949E', fontStyle: 'italic' },
        { token: 'operator', foreground: 'FF7B72' },
      ],
      colors: {
        'editor.background': '#0D1117',
        'editor.foreground': '#C9D1D9',
        'editorLineNumber.foreground': '#6E7681',
        'editorLineNumber.activeForeground': '#C9D1D9',
        'editor.selectionBackground': '#264F78',
        'editor.lineHighlightBackground': '#161B22',
      }
    });

    monaco.editor.defineTheme('wisam-light', {
      base: 'vs',
      inherit: true,
      rules: [
        { token: 'keyword', foreground: 'CF222E', fontStyle: 'bold' },
        { token: 'identifier', foreground: '953800' },
        { token: 'number', foreground: '0550AE' },
        { token: 'string', foreground: '0A3069' },
        { token: 'comment', foreground: '#6E7781', fontStyle: 'italic' },
        { token: 'operator', foreground: 'CF222E' },
      ],
      colors: {
        'editor.background': '#FFFFFF',
        'editor.foreground': '#24292F',
        'editorLineNumber.foreground': '#6E7781',
        'editorLineNumber.activeForeground': '#24292F',
        'editor.selectionBackground': '#B3D7FF',
        'editor.lineHighlightBackground': '#F6F8FA',
      }
    });

    // Register completion provider
    monaco.languages.registerCompletionItemProvider('wisam', {
      provideCompletionItems: () => {
        return {
          suggestions: WISAM_SNIPPETS.map(snippet => ({
            label: snippet.label,
            kind: monaco.languages.CompletionItemKind.Snippet,
            insertText: snippet.insertText,
            insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
            documentation: `Wisam snippet: ${snippet.label}`
          }))
        };
      }
    });
  }, []);

  const handleEditorDidMount = useCallback((editor: any) => {
    editorRef.current = editor;
  }, []);

  const runCode = useCallback(() => {
    setOutput(prev => prev + '\n> Running...\n');
    
    // Simulate code execution
    setTimeout(() => {
      setOutput(prev => prev + 'مرحباً، مستخدم!\nالمجموع: 15\n\n> Execution completed.\n');
      toast.success('Code executed successfully!');
    }, 500);
  }, [code]);

  const saveFile = useCallback(() => {
    toast.success('File saved successfully!');
  }, []);

  const createNewFile = useCallback(() => {
    const fileName = prompt('أدخل اسم الملف:');
    if (fileName) {
      toast.success(`Created: ${fileName}`);
    }
  }, []);

  const toggleFolder = useCallback((folderId: string) => {
    setFolders(prev => prev.map(folder => 
      folder.id === folderId 
        ? { ...folder, isOpen: !folder.isOpen }
        : folder
    ));
  }, []);

  const openFile = useCallback((file: FileItem) => {
    setActiveFile(file.name);
    toast.info(`Opened: ${file.name}`);
  }, []);

  return (
    <div className={`h-screen flex flex-col ${darkMode ? 'dark' : ''}`}>
      {/* Header */}
      <header className="h-14 bg-background border-b flex items-center px-4 justify-between">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-2">
            <div className="w-8 h-8 bg-gradient-to-br from-emerald-500 to-teal-600 rounded-lg flex items-center justify-center">
              <Code className="w-5 h-5 text-white" />
            </div>
            <span className="font-bold text-lg">Wisam Editor</span>
          </div>
          <Separator orientation="vertical" className="h-6" />
          <div className="flex items-center gap-2">
            <Button variant="ghost" size="icon" onClick={createNewFile}>
              <FilePlus className="w-4 h-4" />
            </Button>
            <Button variant="ghost" size="icon" onClick={() => {}}>
              <FolderOpen className="w-4 h-4" />
            </Button>
            <Button variant="ghost" size="icon" onClick={saveFile}>
              <Save className="w-4 h-4" />
            </Button>
            <Separator orientation="vertical" className="h-6" />
            <Button variant="default" size="sm" onClick={runCode} className="gap-2">
              <Play className="w-4 h-4" />
              تشغيل
            </Button>
          </div>
        </div>

        <div className="flex items-center gap-2">
          <Button 
            variant="ghost" 
            size="icon" 
            onClick={() => setDarkMode(!darkMode)}
          >
            {darkMode ? <Sun className="w-4 h-4" /> : <Moon className="w-4 h-4" />}
          </Button>
          <Dialog>
            <DialogTrigger asChild>
              <Button variant="ghost" size="icon">
                <Settings className="w-4 h-4" />
              </Button>
            </DialogTrigger>
            <DialogContent>
              <DialogHeader>
                <DialogTitle>الإعدادات</DialogTitle>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="flex items-center justify-between">
                  <span>حجم الخط</span>
                  <Input type="number" defaultValue={14} className="w-20" />
                </div>
                <div className="flex items-center justify-between">
                  <span>المظهر</span>
                  <Button variant="outline" size="sm">
                    {darkMode ? 'داكن' : 'فاتح'}
                  </Button>
                </div>
              </div>
            </DialogContent>
          </Dialog>
        </div>
      </header>

      {/* Main Content */}
      <ResizablePanelGroup className="flex-1">
        {/* Sidebar - File Explorer */}
        <ResizablePanel defaultSize={20} minSize={15} maxSize={30}>
          <div className="h-full bg-background border-r flex flex-col">
            <div className="p-3 border-b flex items-center justify-between">
              <span className="font-semibold text-sm">المستكشف</span>
              <div className="flex gap-1">
                <Button variant="ghost" size="icon" className="w-6 h-6">
                  <FilePlus className="w-3 h-3" />
                </Button>
                <Button variant="ghost" size="icon" className="w-6 h-6">
                  <Folder className="w-3 h-3" />
                </Button>
              </div>
            </div>
            <ScrollArea className="flex-1">
              <div className="p-2">
                {folders.map(folder => (
                  <div key={folder.id}>
                    <button
                      onClick={() => toggleFolder(folder.id)}
                      className="flex items-center gap-1 w-full p-1 hover:bg-accent rounded text-sm"
                    >
                      <ChevronRight 
                        className={`w-3 h-3 transition-transform ${folder.isOpen ? 'rotate-90' : ''}`} 
                      />
                      <Folder className="w-4 h-4 text-yellow-500" />
                      <span>{folder.name}</span>
                    </button>
                    {folder.isOpen && (
                      <div className="mr-4 border-r pr-2">
                        {folder.files.map(file => (
                          <button
                            key={file.id}
                            onClick={() => openFile(file)}
                            className={`flex items-center gap-2 w-full p-1 hover:bg-accent rounded text-sm ${
                              activeFile === file.name ? 'bg-accent' : ''
                            }`}
                          >
                            <FileText className="w-4 h-4 text-blue-500" />
                            <span>{file.name}</span>
                          </button>
                        ))}
                      </div>
                    )}
                  </div>
                ))}
              </div>
            </ScrollArea>
          </div>
        </ResizablePanel>

        <ResizableHandle />

        {/* Editor */}
        <ResizablePanel defaultSize={80}>
          <ResizablePanelGroup>
            <ResizablePanel defaultSize={consoleOpen ? 70 : 100}>
              <div className="h-full flex flex-col">
                {/* Tabs */}
                <div className="flex items-center bg-background border-b">
                  <div className="flex-1 flex">
                    <div className="flex items-center gap-2 px-3 py-2 bg-accent border-t-2 border-primary">
                      <FileText className="w-4 h-4" />
                      <span className="text-sm">{activeFile}</span>
                      <Button variant="ghost" size="icon" className="w-4 h-4 ml-2">
                        <X className="w-3 h-3" />
                      </Button>
                    </div>
                  </div>
                </div>

                {/* Editor */}
                <div className="flex-1">
                  <Editor
                    height="100%"
                    language="wisam"
                    theme={darkMode ? 'wisam-dark' : 'wisam-light'}
                    value={code}
                    onChange={(value) => setCode(value || '')}
                    beforeMount={handleEditorWillMount}
                    onMount={handleEditorDidMount}
                    options={{
                      minimap: { enabled: true },
                      fontSize: 14,
                      lineNumbers: 'on',
                      roundedSelection: false,
                      scrollBeyondLastLine: false,
                      readOnly: false,
                      automaticLayout: true,
                      tabSize: 4,
                      insertSpaces: true,
                      wordWrap: 'on',
                      folding: true,
                      renderLineHighlight: 'all',
                      matchBrackets: 'always',
                      autoIndent: 'full',
                      formatOnPaste: true,
                      formatOnType: true,
                    }}
                  />
                </div>
              </div>
            </ResizablePanel>

            {consoleOpen && (
              <>
                <ResizableHandle />
                <ResizablePanel defaultSize={30} minSize={20}>
                  <div className="h-full bg-background border-t flex flex-col">
                    <div className="flex items-center justify-between px-3 py-2 border-b bg-muted/50">
                      <div className="flex items-center gap-2">
                        <Terminal className="w-4 h-4" />
                        <span className="text-sm font-medium">الطرفية</span>
                      </div>
                      <div className="flex gap-1">
                        <Button 
                          variant="ghost" 
                          size="sm" 
                          className="h-6 text-xs"
                          onClick={() => setOutput('')}
                        >
                          مسح
                        </Button>
                        <Button 
                          variant="ghost" 
                          size="icon" 
                          className="w-6 h-6"
                          onClick={() => setConsoleOpen(false)}
                        >
                          <X className="w-3 h-3" />
                        </Button>
                      </div>
                    </div>
                    <ScrollArea className="flex-1 p-3">
                      <pre className="text-sm font-mono whitespace-pre-wrap">
                        {output || '> Ready...\n'}
                      </pre>
                    </ScrollArea>
                  </div>
                </ResizablePanel>
              </>
            )}
          </ResizablePanelGroup>
        </ResizablePanel>
      </ResizablePanelGroup>

      {/* Status Bar */}
      <footer className="h-6 bg-primary text-primary-foreground flex items-center px-3 text-xs justify-between">
        <div className="flex items-center gap-4">
          <span>master*</span>
          <span>0 errors, 0 warnings</span>
        </div>
        <div className="flex items-center gap-4">
          <span>Ln 12, Col 34</span>
          <span>UTF-8</span>
          <span>Wisam</span>
          <span>Spaces: 4</span>
        </div>
      </footer>
    </div>
  );
}
