from PIL import Image
import numpy as np

image_path = "textures.png"
image = Image.open(image_path)

image = image.convert('RGB')

output_file = "textures.bin"

texture_size = 64
tilemap_size = 512
textures_count = int(tilemap_size / texture_size)

with open(output_file, 'wb') as f:
    y_offset = 0
    for texture_row in range(textures_count):
        x_offset = 0
        for texture_column in range(textures_count):
            for pixel in range(texture_size * texture_size):
                x = (pixel % texture_size) + x_offset
                y = int(pixel / texture_size) + y_offset

                r, g, b = image.getpixel((x, y))

                f.write(bytes([r]))
                f.write(bytes([g]))
                f.write(bytes([b]))
            x_offset += texture_size
        y_offset += texture_size

    
print(f"RGB values written to {output_file}")

# with open(output_file, 'rb') as f:
#     # read texture file into buffer
#     buffer = np.fromfile(f, dtype=np.uint8)

#     # check if buffer file is as expected
#     if buffer.size != tilemap_size * tilemap_size * 3:
#         raise ValueError(f"Unexpected file size: {buffer.size} bytes. Expected: {tilemap_size * tilemap_size * 3} bytes.")

#     # create a u32 texture array and fill with zeroes
#     texture = np.zeros(tilemap_size * tilemap_size, dtype=np.uint32)

#     # convert from 8 bits to 32 bit
#     for p in range(tilemap_size * tilemap_size):
#         i = p * 3
#         texture[p] = (0 << 24) | (buffer[i] << 16) | (buffer[i + 1] << 8) | buffer[i + 2]

#     # creates new image and writes colors into it
#     image = Image.new('RGB', (texture_size, texture_size))
#     for p in range(texture_size * texture_size):
#         x = p % texture_size
#         y = int(p / texture_size)


#         # a = np.zeros_like(texture, dtype=np.uint8)
#         # r = np.zeros_like(texture, dtype=np.uint8)
#         # g = np.zeros_like(texture, dtype=np.uint8)
#         # b = np.zeros_like(texture, dtype=np.uint8)

#         # Extract ARGB channels
#         a = 0 >> 24
#         r = (texture[p + 2048] >> 16) & 0xFF
#         g = (texture[p+2048] >> 8) & 0xFF
#         b = texture[p+2048] & 0xFF

#         image.putpixel((x, y), (r, g, b))
        
#         # Move to the next set of RGB values (3 bytes)


# # Save the image as PNG
# output_file = 'output.png'
# image.save(output_file)
# print("saved image")