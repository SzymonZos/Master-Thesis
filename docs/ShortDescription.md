# Shot description of Master Thesis

## Intro

Image compression in JPEG 2000 is performed using discrete wavelet transform (DWT) in a standard way, i.e. in a standard way the image is decomposed into subbands using a pair of low- and high-pass filters (multiple for higher DWT orders). On the other hand, part 2 of the JPEG 2000 standard contains extensions that allow modifying DWT, i.e. decomposing the image into subbands of different shapes and using a different pair of filters (the same pair for all subbands of the image). Using these extensions, e.g., one can get cetrain variants of the skipped steps DWT transform (SS-DWT) that usually are better for non-photographic images (also known as screen content images). Reversible histogram packing (HP) is also possible, which significantly improves the compression ratio when the image histogram is sparse (unused levels appear between frequently used brightness levels). Using these extensions, one can adaptively adjust the transform for a given image to improve the compression ratio, and the compressed image will still be correctly decoded by a decompressor compatible with JPEG 2000 part 2.

## Schedule

1. Introduction: image processing and compression, JPEG 2000 algorithm, DWT, SS-DWT, HP, JPEG 2000 implementations (Kakadu).
2. Development of the concept of initial heuristics allowing to study the effects of DWT modifications compliant with JPEG 2000, such as heuristically generated different image decompositions into subbands for grayscale and color images, the use of different filters and HP.
3. Conducting preliminary tests and selecting modifications or their variants to be included in the final heuristics.
4. Development of multi-threaded optimized implementation of final heuristics.
5. Research on final heuristics - comparison in terms of obtained compression ratio and time with: unmodified JPEG 2000, SS-DWT transformation, and the transformation determined by an exhaustive search.
6. Summary and conclusions.

## Literature

* Literature in the field of image processing, image compression, the JPEG 2000 standard.
* Literature in the field of concurrent programming as well as threads and other advanced mechanisms of the C++ language.
* Starosolski, R.: Skipping selected steps of DWT computation in lossless JPEG 2000 for improved bitrates, https://doi.org/10.1371/journal.pone.0168704
* Starosolski, R.: A practical application of skipped steps DWT in JPEG 2000 part 2-compliant compressor, http://sun.aei.polsl.pl/~rstaros/papers/s2018-bdas-AAM.pdf

## Misc

The task will be to develop, implement, test, and select the final form of heuristics, which will determine the JPEG 2000 part 2 standard-compliant transform for the given image. Heuristics should be fast, can use JPEG 2000 coding estimation using entropy, can be greedy and use trial and error approach to some extent. The implementation of heuristics should be done in C ++ using modern language mechanisms including threads to effectively use the multi-core CPU architectures. The practical result of the work will be a tool (it can be made available to the public) that will quickly determine the transform for a given image and invoke the JPEG 2000 encoder with this transform, enabling the improvement of the lossless JPEG 2000 compression ratio at the cost of a slight increase in the time of the entire compression process.
