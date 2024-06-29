from PIL import Image

image_path = "wall1.png"
image = Image.open(image_path)
width, height = image.size

image = image.convert('RGB')

# Open binary file for writing
output_file = "wall1.bin"  # Output binary file path
with open(output_file, 'wb') as f:
    for y in range(height):
        for x in range(width):
            r, g, b = image.getpixel((x, y))
            # f.write(bytes([a]))
            f.write(bytes([r]))
            f.write(bytes([g]))
            f.write(bytes([b]))

print(f"RGB values written to {output_file}")