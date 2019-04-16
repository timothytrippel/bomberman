module d_ff_tb;
 
reg clk, reset, d;
wire q, q_bar;

d_ff d0(
	.reset(reset),
	.d (d),
	.clk (clk),
	.q (q),
	.q_bar (q_bar)
);
 
initial begin
	$dumpfile("d_ff.vcd");
	$dumpvars(0, d0);
	$monitor("clk=%b, d=%b, q=%b, q_bar=%b", clk, d, q, q_bar);
	clk = 0;
	d = 1;
	#10 d = 0;
	#20 $finish;
end
 
always begin
	#5 clk = !clk;
end

endmodule
