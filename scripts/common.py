from collections.abc import Iterable
from pathlib import Path


def find_files(
    search_paths: Iterable[str],
    file_patterns: Iterable[str],
    exclude_paths: Iterable[str]
) -> set[Path]:
    matching_files = []
    for search_path in search_paths:
        path = Path(search_path)
        for pattern in file_patterns:
            matching_files.extend(path.rglob(pattern))

    filtered_files = {
        file for file in matching_files
        if not any(str(file.parent).startswith(path) for path in exclude_paths)
    }

    return filtered_files
