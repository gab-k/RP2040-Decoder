"""
Script for automatically generating .svg files for documentation

How to use:
1. Install Inkscape and add to PATH
2. Install KiCad and add to PATH
3. Install PyPDF2
4. Copy the schematic_theme.json file into the following directory:
    Linux: ~.config/kicad/<version>/colors
    Windows: %appdata%/kicad/<version>/colors
"""


import subprocess
import os

# Output directory path
svg_dir = "../docs/svg/hw/USB/"
# Temp directory path
temp_dir = "temp/"
# .kicad_sch path/filename
kicad_sch_filename = "../KiCad/USB/RP2040-Decoder.kicad_sch"

# Define crop areas with p1 (bottom-left) and p2 (top-right) points and custom pdf_name
crop_areas = {
    "Input Stage": {
        "p1": (60, 440),  # Bottom-left corner (x1, y1)
        "p2": (430, 560),  # Top-right corner (x2, y2)
        "name": "Input_Stage"
    },
    "Motor Driver": {
        "p1": (600, 125),
        "p2": (810, 315),
        "name": "Motor_Driver"
    },
    "Auxillary Outputs": {
        "p1": (620, 395),
        "p2": (810, 555),
        "name": "Aux_Outputs"
    },
    "Microcontroller": {
        "p1": (40, 20),
        "p2": (600, 420),
        "name": "Microcontroller"
    }
}

# 1. Create temp directory
try:
    os.mkdir(temp_dir)
    print(f"Directory '{temp_dir}' created successfully.")
except FileExistsError:
    print(f"Directory '{temp_dir}' already exists.")
except PermissionError:
    print(f"Permission denied: Unable to create '{temp_dir}'.")
except Exception as e:
    print(f"An error occurred: {e}")

# 2. Export from KiCad with and without drawing sheet
subprocess.run(
    ["kicad-cli", "sch", "export", "pdf", "--no-background-color", "--exclude-drawing-sheet", "--theme", "schematic_theme",
     "--output", temp_dir + "RP2040-Decoder_temp.pdf", kicad_sch_filename],
    check=True
)
subprocess.run(
    ["kicad-cli", "sch", "export", "svg", "--no-background-color", "--theme", "schematic_theme",
     "--output", svg_dir, kicad_sch_filename],
    check=True
)


# 3. Crop the PDF for each specified crop area
for crop_key, crop_data in crop_areas.items():
    p1 = crop_data["p1"]  # (x1, y1)
    p2 = crop_data["p2"]  # (x2, y2)
    pdf_path = temp_dir + crop_data["name"] + ".pdf" # Name of the output cropped PDF
    
    subprocess.run(
        ["python", "crop_pdf.py", temp_dir + "RP2040-Decoder_temp.pdf", pdf_path, str(p1[0]), str(p1[1]), str(p2[0]), str(p2[1])],
        check=True
    )
    print(f"Cropped part {crop_key} saved as {pdf_path}")

# 4. Convert each cropped PDF to SVG using Inkscape
for crop_key, crop_data in crop_areas.items():
    svg_path = svg_dir + crop_data["name"] + ".svg"
    pdf_path = temp_dir + crop_data["name"] + ".pdf"
    
    subprocess.run(
        ["inkscape", "--export-type=svg", "--export-filename=" + svg_path, pdf_path],
        check=True
    )
    print(f"Converted {pdf_path} to {svg_path}")

# 5. Set transparency for each SVG
for crop_key, crop_data in crop_areas.items():
    svg_path = svg_dir + crop_data["name"] + ".svg"
    subprocess.run(
        ["python", "set_opacity.py", svg_path, svg_path],
        check=True
    )
    print(f"Set transparency for {svg_path}")

# 6. Clean up the temporary PDF files and attempt to delete temp directory
for crop_key, crop_data in crop_areas.items():
    pdf_path = temp_dir + crop_data["name"] + ".pdf"
    try:
        os.remove(pdf_path)  # Attempt to delete the cropped PDF
        print(f"Deleted temporary file: {pdf_path}")
    except FileNotFoundError:
        print(f"File not found: {pdf_path}")
    except PermissionError:
        print(f"Permission denied: Unable to delete '{pdf_path}'.")
    except Exception as e:
        print(f"An error occurred while deleting '{pdf_path}': {e}")

temp_file = temp_dir + "RP2040-Decoder_temp.pdf"
try:
    os.remove(temp_file)  # Attempt to delete the final temporary file
    print(f"Deleted temporary file: {temp_file}")
except FileNotFoundError:
    print(f"File not found: {temp_file}")
except PermissionError:
    print(f"Permission denied: Unable to delete '{temp_file}'.")
except Exception as e:
    print(f"An error occurred while deleting '{temp_file}': {e}")


try:
    os.rmdir(temp_dir)
    print(f"Directory '{temp_dir}' deleted successfully.")
except FileNotFoundError:
    print(f"Directory '{temp_dir}' does not exist.")
except OSError:
    print(f"Directory '{temp_dir}' is not empty or cannot be removed.")
except PermissionError:
    print(f"Permission denied: Unable to delete '{temp_dir}'.")
except Exception as e:
    print(f"An error occurred: {e}")