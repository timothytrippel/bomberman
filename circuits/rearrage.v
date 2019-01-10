// Test a signal assigned for overlapping slices
// Output should just be one counter in
module rearrage(input [3:0] in, output [3:0] out);
  assign in[0] = out[1];
  assign in[1] = out[0];
  assign in[2] = out[3];
  assign in[3] = out[2];
endmodule
