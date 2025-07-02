import argparse
import re
from pathlib import Path


def encode_md_link(path_str: str) -> str:
    """
    Encode a Markdown filepath according to Doxygen's rules:
    - prefix with 'md_'
    - replace '/' with '_2'
    - double underscores '__' for underscores '_'
    """
    # Separate anchor if present
    if '#' in path_str:
        filepath, anchor = path_str.split('#', 1)
        anchor = '#' + anchor
    else:
        filepath, anchor = path_str, ''

    # Remove leading slash and .md suffix
    filepath = filepath.removeprefix('/')
    if not filepath.endswith('.md'):
        return path_str  # Not a .md file
    filepath = filepath.removesuffix('.md')

    # Encode path separators and underscores
    parts = filepath.split('/')
    encoded_parts = []
    for part in parts:
        # Double underscores
        part = part.replace('_', '__')
        encoded_parts.append(part)
    encoded_path = '_2'.join(encoded_parts)

    # Add md_ prefix and .html suffix
    return f"md_{encoded_path}.html{anchor}"


def process_md_refs(content: str) -> str:
    pattern = re.compile(r'href="([^"]+\.md(?:#[^"]*)?)"')

    def replacer(match):
        link = match.group(1)
        new_link = encode_md_link(link)
        return f'href="{new_link}"'

    return pattern.sub(replacer, content)


def process_gfm(content: str) -> str:
    """
    Replace GFM-style callouts with styled inline HTML spans.
    """
    callouts = {
        '[!NOTE]':      ('Note', '#1e90ff', '&#9432;'),   # ℹ
        '[!TIP]':       ('Tip', '#28a745', '&#128161;'),  # 💡
        '[!IMPORTANT]': ('Important', '#d63384', '&#10071;'),  # ❗
        '[!WARNING]':   ('Warning', '#fd7e14', '&#9888;'),     # ⚠
        '[!CAUTION]':   ('Caution', '#dc3545', '&#9940;'),     # 🚫
    }

    for tag, (label, color, icon) in callouts.items():
        replacement = (
            f'<span style="color: {color}; font-weight: bold; font-size: 1.1em;">'
            f'{icon} {label}:</span>'
        )
        content = content.replace(tag, replacement)

    return content


def process_file(f: Path):
    content = f.read_text(encoding='utf-8')
    content = process_md_refs(content)
    content = process_gfm(content)
    f.write_text(content, encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description='Postprocess Doxygen HTML files to fix links.')
    parser.add_argument('directory', type=Path, help='Root directory of generated HTML files')

    args = parser.parse_args()

    for f in args.directory.rglob('*.html'):
        process_file(f)


if __name__ == '__main__':
    main()
