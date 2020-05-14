% The test function gives an example of keypoint extraction using the
    % methods :
    % - Harris
    %
    % Example
    % =======

    img11 = imread('yy_black.jpg');
    img1 = rgb2gray(img11);
    img1 = double(img1(:,:));
    pt1 = kp_harris(img1);
 
    img21 = imread('yy_right.jpg');
    img2 = rgb2gray(img21);
    img2 = double(img2(:,:));
    pt2 = kp_harris(img2);
    %draw(img21,pt2,'Harris');
    
    % match key points within two pictures.
    result = match(img1,pt1,img2,pt2);
    result(1,intersect(find(result(1,:) > 0),find(result(2,:) == 0))) = 0;
    %result
    %pause;
    while(length(find(result(1,:)>0)) > 3)     
        result
%        draw2(img11,img21,pt1,pt2,result);
        %find(result(1,:)>0)
        pause;
        [index index] = max(result(2,:));
        result(1,index(1)) = 0;
        result(2,index(1)) = 0;
        %result(1,I(1)) = result(2,I(1)) = 0
    end
%    draw2(img11,img21,pt1,pt2,result);

S(:,1:173,:)=img11(:,1:173,:);
S(:,174:345,:)=img21(:,11:182,:);
  imshow(S);
