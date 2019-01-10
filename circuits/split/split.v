module split(input [1:0] a, input[1:0] b, output [1:0] o);
  assign o[0] = a[0] & b[1];
  assign o[1] = a[1] & b[0];
endmodule
