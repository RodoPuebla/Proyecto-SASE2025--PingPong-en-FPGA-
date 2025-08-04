library ieee;
use ieee.std_logic_1164.all;

entity Horizontal is

	port(
		clk			: in	std_logic;
		cntH			: in	integer range 0 to 800;
--		cntV			: in	integer;
		reset			: in	std_logic;
		Hs					: out std_logic;
		H_blanck		: out	std_logic
	);

end entity;

architecture rtl of Horizontal is

	type state_type is (a, b, c, d);

	signal state   : state_type;

begin

	process (clk, reset)
	begin
		if reset = '0' then
			state <= a;
		elsif (rising_edge(clk)) then
			case state is
				when a=>
					if cntH <95 then
						state <= a;
					else
						state <= b;
					end if;
				when b=>
					if cntH <144 then
						state <= b;
					else
						state <= c;
					end if;
				when c=>
					if cntH <783 then
						state <= c;
					else
						state <= d;
					end if;
				when d =>
					if cntH <800 then
						state <= d;
					else
						state <= a;
					end if;
			end case;
		end if;
	end process;

	process (state)
	begin
		case state is
			when a =>
				Hs <= '0';
				H_blanck	<= '0';
			when b =>
				Hs <=  '1';
				H_blanck	<= '0';
				when c =>
				Hs <= '1';
				H_blanck	<= '1';
				when d =>
				Hs <= '1';
				H_blanck	<= '0';
				end case;
	end process;

end rtl;
