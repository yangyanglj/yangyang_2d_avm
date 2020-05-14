
function regionValue = getRegionValue(img,pt)
    len = size(pt,1);
    regionValue = zeros(8,len);
    maxX = size(img,1);
    maxY = size(img,2);
    for i=1:len
       x = pt(i,1);
       y = pt(i,2);
       %1
       if(x-1<1||y-1<1)
           regionValue(1,i)=100;
       else
           regionValue(1,i)=img(x,y)-img(x-1,y-1);
       end
       %2
       if(x-1<1)
           regionValue(2,i)=200;
       else
           regionValue(2,i)=img(x,y)-img(x-1,y);
       end
       %3
       if(x-1<1||y+1>maxY)
           regionValue(3,i)=300;
       else
           regionValue(3,i)=img(x,y)-img(x-1,y+1);
       end
       %4
       if(y+1>maxY)
           regionValue(4,i)=400;
       else
           regionValue(4,i)=img(x,y)-img(x,y+1);
       end
       %5
       if(x+1>maxX||y+1>maxY)
           regionValue(5,i)=500;
       else
           regionValue(5,i)=img(x,y)-img(x+1,y+1);
       end
       %6
       if(x+1>maxX)
           regionValue(6,i)=600;
       else
           regionValue(6,i)=img(x,y)-img(x+1,y);
       end
       %7
       if(x+1>maxX||y-1<1)
           regionValue(7,i)=700;
       else
           regionValue(7,i)=img(x,y)-img(x+1,y-1);
       end
       %8
       if(y-1<1)
           regionValue(8,i)=800;
       else
           regionValue(8,i)=img(x,y)-img(x,y-1);
       end
    end
    
end
 