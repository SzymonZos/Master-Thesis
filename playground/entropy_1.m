function [out, nzprobs] = entropy_1(img)
    counts = histcounts(img, 0:256);
    probs = counts ./ numel(img);
    nzprobs = probs(probs>0);
    out = sum(nzprobs .* log2(1./nzprobs));
end
