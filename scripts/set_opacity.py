import re
import argparse

def set_opacity(input_file, output_file):
    """
    Replace all occurrences of 'fill-opacity:1' with 'fill-opacity:0',
    except when preceded by 'fill:#ffffc2'.

    :param input_file: Path to the input SVG file.
    :param output_file: Path to save the modified SVG file.
    """
    try:
        # Read the content of the input SVG file
        with open(input_file, 'r') as file:
            svg_content = file.read()

        # Regex to match 'fill-opacity:1' and not preceded by 'fill:#ffffc2'
        pattern = r'(?<!fill:#ffffc2;)fill-opacity:1'
        modified_content = re.sub(pattern, 'fill-opacity:0', svg_content)

        # Save the modified content to the output file
        with open(output_file, 'w') as file:
            file.write(modified_content)

        print(f"Modified SVG saved as: {output_file}")

    except Exception as e:
        print(f"An error occurred: {e}")

def main():
    # Set up the argument parser
    parser = argparse.ArgumentParser(
        description="Modify SVG files by replacing 'fill-opacity:1' with 'fill-opacity:0', "
                    "except when preceded by 'fill:#ffffc2'."
    )
    parser.add_argument('input_file', type=str, help="Path to the input SVG file.")
    parser.add_argument('output_file', type=str, help="Path to save the modified SVG file.")
    
    # Parse the arguments
    args = parser.parse_args()

    # Call the function with the provided file paths
    set_opacity(args.input_file, args.output_file)

if __name__ == "__main__":
    main()
