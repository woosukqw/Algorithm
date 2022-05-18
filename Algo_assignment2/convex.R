#! /usr/bin/env Rscript
png("convex.png", width=700, height=700)
plot(1:10000, 1:10000, type="n")

#points
points(1786,1902)
points(2435,3618)
points(2828,4654)
points(2915,5311)
points(3236,4531)
points(3307,2156)
points(4446,7502)
points(6287,3533)
points(6777,5749)
points(6857,3347)
points(7532,8636)
points(8219,4772)
points(8490,17)
points(9401,6632)
points(9610,7110)

#line segments
segments(1786,1902,2915,5311)
segments(2915,5311,4446,7502)
segments(4446,7502,7532,8636)
segments(7532,8636,9610,7110)
segments(9610,7110,8490,17)
segments(8490,17,1786,1902)
dev.off()
