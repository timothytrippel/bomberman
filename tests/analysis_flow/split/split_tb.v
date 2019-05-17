module split_tb;
    reg[1:0]  a;
    reg[1:0]  b;
    wire[1:0] o;
    
    initial begin
        $dumpfile("split.vcd");
        $dumpvars(0, s);
        $monitor("a=%b, b=%b", a, b);

        a = 2'b00;
        b = 2'b00;
        #5;

        repeat(4) begin
            repeat(3) begin
                b = b + 1;
                #5;
            end
            a = a + 1;
            b = 2'b00;
            #5;
        end

        $finish;
    end

    split s(a, b, o);
endmodule
