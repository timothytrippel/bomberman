module basic_2signal_ff(
	input clk, 
	input rst, 
	input en, 
	input d1, 
	input d2, 
	output reg q1,
	output reg q2
);

	wire next_q1;
	wire next_q2;

	assign next_q1 = en ? d1 : q1;
	assign next_q2 = en ? d2 : q2;

	always @(posedge clk) begin
		if (rst) begin
			q1 <= 1'b0;
			q2 <= 1'b0;
		end
		else begin
			q1 <= next_q1;
			q2 <= next_q2;
		end	
	end
endmodule
