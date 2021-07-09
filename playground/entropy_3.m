function out = entropy_3(img)
    function y = proc(chunk)
        if(chunk == 0)
            y = 0;
        else
            y = chunk * log2(1/chunk);    
        end
    end
    out = sum(arrayfun(@proc, double(img) ./ double(sum(img))));
end
