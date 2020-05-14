from PIL import Image
import math

def fish_eye_dis(img):
    "fish eye distortion"
    width_in, height_in = img.size;
    im_out = Image.new("RGB",(width_in,height_in));
    radius = max(width_in, height_in)/2;
	#assume the fov is 180
	#R = f*theta
    lens = radius*2/math.pi;
    for i in range(width_in):
        for j in range(height_in):
            #offset to center
            x = i - width_in/2;
            y = j - height_in/2;
            r = math.sqrt(x*x + y*y);
            theta = math.atan(r/radius);
            if theta<0.00001:
                k = 1;
            else:
                k = lens*theta/r;
                
            src_x = x*k;
            src_y = y*k;
            src_x = src_x+width_in/2;
            src_y = src_y+height_in/2;
            pixel = im.getpixel((src_x,src_y));
            im_out.putpixel((i,j),pixel);

    return im_out;



    
if __name__=="__main__":
    input_name = "BACK_0.bmp";
    output_name = "BACK_0_out.bmp";
    im = Image.open(input_name);
    img_out = fish_eye_dis(im);
    img_out.save(output_name);
    
    print "fish eye distortion completely, save image to %s" % output_name
