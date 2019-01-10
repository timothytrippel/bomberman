module d_ff(reset, d, clk, q, q_bar);
  input d, clk, reset;
  output q, q_bar;
  reg q;
  reg q_bar;

  always @ (posedge clk)
  begin
    if (reset) begin
      q <= 1'b0;
      q_bar <= 1'b1;
    end else begin
      q <= d;
      q_bar <= !d;
    end
  end
endmodule
