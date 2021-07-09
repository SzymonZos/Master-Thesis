function [out, nzprobs] = entropy_2(img)
    counts = histc(img(:), 0:255);
    probs = counts ./ numel(img);
    nzprobs = probs(probs>0);
    out = sum(nzprobs .* log2(1./nzprobs));
end
