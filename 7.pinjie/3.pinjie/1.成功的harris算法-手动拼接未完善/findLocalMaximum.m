
function [row,col,max_local] = findLocalMaximum(val,radius)
    % Determine the local maximum of a given value
    %
    %
    % INPUT
    % =====
    % val    : the NxM matrix containing values
    % radius : the radius of the neighborhood
    %
    % OUTPUT
    % ======
    % row       : the row position of the local maxima
    % col       : the column position of the local maxima
    % max_local : the NxM matrix containing values of val on unique local maximum
    %
    % EXAMPLE
    % =======
    % [l,c,m] = findLocalMaximum(img,radius);
    
    % FIND UNIQUE LOCAL MAXIMA USING FILTERING (FAST)
    mask  = fspecial('disk',radius)>0;
    nb    = sum(mask(:));
    highest          = ordfilt2(val, nb, mask);
    second_highest   = ordfilt2(val, nb-1, mask);
    index            = highest==val & highest~=second_highest;
    max_local        = zeros(size(val));
    max_local(index) = val(index);
    [row,col]        = find(index==1);
end

