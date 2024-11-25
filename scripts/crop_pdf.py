##############################
#                            #
#                            #
#   -----p2                  #
#   |     |                  #
#   |     | <- Crop Window   #  <- Whole PDF Window
#   |     |                  #
#   p1----|                  #
#                            #
##############################

# Coordinate origin is at bottom left of PDF window  
#
# y
# î
# |
# |
# +-----> x

import argparse
from PyPDF2 import PdfReader, PdfWriter

def crop_pdf(input_path, output_path, p1, p2):
    """
    Crop a PDF file based on the given bottom-left (p1) and top-right (p2) coordinates.
    
    Parameters:
    - input_path (str): Path to the input PDF file.
    - output_path (str): Path to save the cropped PDF file.
    - p1 (tuple): Bottom-left corner of the crop window (x, y).
    - p2 (tuple): Top-right corner of the crop window (x, y).
    """
    # Validate coordinates
    if p1[0] >= p2[0]:
        raise ValueError("Invalid x dimensions: p1[0] must be less than p2[0]")
    if p1[1] >= p2[1]:
        raise ValueError("Invalid y dimensions: p1[1] must be less than p2[1]")
    
    # Open the input PDF
    with open(input_path, "rb") as in_f:
        input_pdf = PdfReader(in_f)
        output_pdf = PdfWriter()
        num_pages = len(input_pdf.pages)
        print(f"Document has {num_pages} pages.")
        
        # Loop through all pages
        for i in range(num_pages):
            page = input_pdf.pages[i]

            # Set the cropbox properties for the page
            page.cropbox.lower_left = p1  # Bottom-left corner
            page.cropbox.upper_right = p2  # Top-right corner

            # Debug information about the crop box
            print(f"Page {i + 1}:")
            print(f" - Cropbox Lower Left: {page.cropbox.lower_left}")
            print(f" - Cropbox Upper Right: {page.cropbox.upper_right}")

            # Add the modified page to the writer
            output_pdf.add_page(page)
        
        # Write the final output
        with open(output_path, "wb") as out_f:
            output_pdf.write(out_f)

    print(f"Cropped PDF saved as {output_path}")

# Add argument parsing to handle command-line inputs
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Crop a PDF file based on coordinates.")
    parser.add_argument("input_path", type=str, help="Path to the input PDF file.")
    parser.add_argument("output_path", type=str, help="Path to save the cropped PDF file.")
    parser.add_argument("p1", type=float, nargs=2, help="Bottom-left corner of the crop window (x, y).")
    parser.add_argument("p2", type=float, nargs=2, help="Top-right corner of the crop window (x, y).")
    args = parser.parse_args()

    # Convert command-line inputs for coordinates
    bottom_left = tuple(args.p1)
    top_right = tuple(args.p2)

    # Call the crop function
    try:
        crop_pdf(args.input_path, args.output_path, bottom_left, top_right)
    except ValueError as e:
        print(f"Error: {e}")
