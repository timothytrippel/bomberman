module same(input a, input b, input c, input d, output[1:0] o);
  wire [1:0] ab;
  wire [1:0] cd;

  assign ab[0] = a;
  assign ab[1] = b;
  assign cd[0] = c;
  assign cd[1] = d;

  assign o[0] = ~a & b & ~c & d;
  assign o[1] = ~ab[0] & ab[1] & ~cd[0] & cd[1];
endmodule
