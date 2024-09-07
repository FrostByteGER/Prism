# -*- coding: utf-8 -*-

import argparse
from pathlib import Path
import shutil
import os
import subprocess


def copy_folder(source_folder, target_folder, ignored_files=[], delete_existing_target=False):
    print(f"Copying '{source_folder}' to '{target_folder}'")
    source_folder = Path(source_folder)
    if not source_folder.exists() or not source_folder.is_dir():
        print(f"Directory '{source_folder}' doesn't exist or is not a directory")
        return

    target_folder = Path(target_folder)
    if delete_existing_target and target_folder.exists() and target_folder.is_dir():
        print(f"Deleting existing target folder '{target_folder}'")
        shutil.rmtree(target_folder)

    ignored_files = [item.lower() for item in ignored_files]
    try:
        shutil.copytree(source_folder, target_folder, dirs_exist_ok=True, ignore=lambda src, names: {file for file in names if file.lower() in ignored_files})
        print(f"Successfully copied '{source_folder}' to '{target_folder}'")
    except shutil.Error as e:
        print("Error copying directory:")
        for src_path, dst_path, error_message in e.args[0]:
            print(f"Failed to copy {src_path} to {dst_path}: {error_message}")
    except OSError as e:
        print(f"OS error: {e}")
    except Exception as e:
        # Catch-all for any other unexpected errors
        print(f"Unexpected error: {e}")


def compile_shaders(shader_folder, shader_compiler_path, delete_source_files=False):
    print(f"Compiling shaders with compiler '{shader_compiler_path}' in directory '{shader_folder}'")
    shader_folder = Path(shader_folder)
    if not shader_folder.exists() or not shader_folder.is_dir():
        print(f"Directory '{shader_folder}' doesn't exist or is not a directory")
        return

    shader_compiler_path = Path(shader_compiler_path)
    if not shader_compiler_path.exists() or not shader_compiler_path.is_file():
        print(f"Shader Compiler EXE '{shader_compiler_path}' doesn't exist or is not an executable")
        return

    for root, dirs, files in os.walk(shader_folder):
        for file in files:
            if file.lower().endswith(('.vert', '.frag', '.comp')):
                file_path = os.path.join(root, file)

                # Compile the file to SPIR-V
                output_file = f"{file_path}.spv"
                result = subprocess.run([shader_compiler_path, '--target-env=vulkan1.3', file_path, '-o', output_file])
                if result.returncode != 0:
                    print(f"Shader compilation failed with return code {result.returncode}")
                    print(f"Error output: {result.stderr.strip()}")
                    continue

                print(f"Successfully compiled shader file to '{output_file}'")
                if delete_source_files:
                    os.remove(file_path)
                    print(f"Deleted shader source file '{file_path}'")


def event_copy_folder(args):
    copy_folder(args.src, args.dst, args.ignoredFiles, args.deleteExistingTarget)

def event_compile_shaders(args):
    compile_shaders(args.srcFolder, args.compilerPath, args.deleteShaderSources)

def main():
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(dest='command', required=True)
    # COPY command
    copy_parser = subparsers.add_parser('COPYFOLDER', help='Copy files')
    copy_parser.add_argument('src', help='Source folder to copy recursively')
    copy_parser.add_argument('dst', help='Destination where the files and folders will be copied to')
    copy_parser.add_argument('--ignoredFiles', nargs='+', default=[], help='Space-separated list of filenames or extensions to ignore')
    copy_parser.add_argument('--deleteExistingTarget', action=argparse.BooleanOptionalAction, default=False,
                             help='Whether to delete an already existing target folder before copying to it')
    copy_parser.set_defaults(func=event_copy_folder)

    # COMPILE command
    compile_parser = subparsers.add_parser('SHADERCOMPILE', help='Compile shader files')
    compile_parser.add_argument('srcFolder', help='Source folder to compile files in')
    compile_parser.add_argument('compilerPath', help='Path and name of the shader compiler executable')
    compile_parser.add_argument('--deleteShaderSources', action=argparse.BooleanOptionalAction, default=False,
                             help='Whether to delete the original shader source files after compilation')
    compile_parser.set_defaults(func=event_compile_shaders)

    args = parser.parse_args()
    args.func(args)

if __name__ == '__main__':
    main()

