# Contributing Wisam to GitHub Linguist
# المساهمة بلغة وسام في GitHub Linguist

This document explains how to add Wisam to GitHub's official linguist repository.

## Prerequisites / المتطلبات

1. Fork the [github/linguist](https://github.com/github/linguist) repository
2. Clone your fork locally
3. Follow the [contribution guidelines](https://github.com/github/linguist/blob/master/CONTRIBUTING.md)

## Files to Add / الملفات المراد إضافتها

### 1. Update `lib/linguist/languages.yml`

Add the following entry:

```yaml
Wisam:
  type: programming
  color: "#4A90D9"
  extensions:
    - ".wsm"
  tm_scope: source.wisam
  ace_mode: text
  language_id: 999999001  # Use a unique ID
  interpreters:
    - wisam
  aliases:
    - wisam-lang
    - وسام
```

### 2. Add Grammar File

Copy `grammars/wisam.tmLanguage.json` to:
- `vendor/grammars/wisam/` directory

### 3. Add Sample Files

Copy sample files to:
- `samples/Wisam/` directory

### 4. Update `grammars.yml`

Add:
```yaml
- source.wisam
- vendor/grammars/wisam/wisam.tmLanguage.json
```

### 5. Run Tests

```bash
bundle install
bundle exec rake test
```

### 6. Submit Pull Request

Create a PR with:
- Clear description of the language
- Links to documentation
- Popularity evidence (if required)

## Language Popularity / شعبية اللغة

GitHub requires languages to have "sufficient" popularity. Evidence can include:
- Number of repositories using the language
- Community activity
- Documentation and tutorials

## Color Choice / اختيار اللون

The color `#4A90D9` was chosen because:
- It's a professional blue color
- It represents trust and technology
- It's distinct from other language colors

---

## Quick Reference / مرجع سريع

```bash
# Clone linguist
git clone https://github.com/YOUR_USERNAME/linguist.git
cd linguist

# Add Wisam files
cp /path/to/wisam/grammars/wisam.tmLanguage.json vendor/grammars/wisam/
cp /path/to/wisam/samples/*.wsm samples/Wisam/

# Edit lib/linguist/languages.yml (add Wisam entry)
# Edit grammars.yml (add Wisam grammar)

# Test
bundle exec rake test

# Commit and push
git add .
git commit -m "Add Wisam programming language"
git push origin add-wisam-language

# Create PR on GitHub
```

## Contact / تواصل

For questions about Wisam:
- Repository: https://github.com/droy-go/wisam
- Issues: https://github.com/droy-go/wisam/issues
