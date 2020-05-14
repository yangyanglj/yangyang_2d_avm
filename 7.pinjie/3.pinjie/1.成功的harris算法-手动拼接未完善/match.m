
function result = match(img1,pt1,img2,pt2)    
    %得到标定点周围的像素值
    regionValue1 = getRegionValue(img1,pt1);
    len1 = size(regionValue1,2);
    regionValue2 = getRegionValue(img2,pt2);
    len2 = size(regionValue2,2);
    
    %找出最佳匹配点
    result = zeros(2,len2);
    for i=1:len1
        B = regionValue1(:,i);
        %abs(regionValue2-B(:,ones(1,size(regionValue2,2))))
        %sum(abs(regionValue2-B(:,ones(1,size(regionValue2,2)))))
        [value,index] = sort(sum(abs(regionValue2-B(:,ones(1,size(regionValue2,2))))));
        %value(1)
        %index(1)
        %save index and value
        if((result(1,index(1))==0)||(result(2,index(1))>value(1)))
            result(1,index(1))=i;
            result(2,index(1))=value(1);
        end
    end    
end
 