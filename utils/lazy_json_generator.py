import argparse
import os
import json

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='Lazy FS Generator',
                    description='Generates json files for lazy loading of file system data')
    
    parser.add_argument('--input', type=str, help='Input directory containig the files to be processed')
    parser.add_argument('--url', type=str, help='Access path for the files, e.g., http://localhost:8080/')
    parser.add_argument('--output', type=str, help='Output file path for the generated file')
    parser.add_argument('--vpath', type=str, help='Virtual path prefix for the files')

    args = parser.parse_args()
    input_dir = args.input
    url = args.url
    output_file = args.output
    vpath = args.vpath

    # Create output directory if it doesn't exist
    output_dir = os.path.dirname(output_file)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)

    dirs_to_walk = [input_dir]
    all_dirs = set()

    entries = []
    
    # Collect all directories first
    while dirs_to_walk:
        current_dir = dirs_to_walk.pop()
        for entry in os.listdir(current_dir):
            full_path = os.path.join(current_dir, entry)
            if os.path.isdir(full_path):
                dirs_to_walk.append(full_path)
                relative_dir = os.path.relpath(full_path, input_dir)
                virtual_dir = os.path.join(vpath, relative_dir).replace(os.sep, '/')
                all_dirs.add(virtual_dir)
    
    # Create directory structure
    sorted_dirs = sorted(all_dirs, key=lambda x: x.count('/'))
    for virtual_dir in sorted_dirs:
        parent_dir = os.path.dirname(virtual_dir)
        dir_name = os.path.basename(virtual_dir)
        entries.append({'type': 'path', 'parent': parent_dir, 'name': dir_name})

    # Reset dirs_to_walk for file processing
    dirs_to_walk = [input_dir]
    
    # Process files
    while dirs_to_walk:
        current_dir = dirs_to_walk.pop()
        for entry in os.listdir(current_dir):
            full_path = os.path.join(current_dir, entry)
            if os.path.isdir(full_path):
                dirs_to_walk.append(full_path)
            elif os.path.isfile(full_path):
                relative_path = os.path.relpath(full_path, input_dir)
                virtual_path = os.path.join(vpath, relative_path).replace(os.sep, '/')
                file_url = url.rstrip('/') + '/' + relative_path.replace(os.sep, '/')
                entries.append({'type': 'file', 'parent': os.path.dirname(virtual_path), 'name': os.path.basename(virtual_path), 'url': file_url})

    # Write to output file
    json.dump(entries, open(output_file, 'w'), indent=2)