library ieee;
use ieee.std_logic_1164.all;

entity Vertical is

	port(
		clk			: in	std_logic;
--		cntH			: in	integer;
		cntV			: in	integer range 0 to 525;
		reset			: in	std_logic;
		Vs				: out std_logic;
		V_blanck		: out	std_logic
	);

end entity;

architecture rtl of Vertical is

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
					if cntV <2 then
						state <= a;
					else
						state <= b;
					end if;
				when b=>
					if cntV <35 then
						state <= b;
					else
						state <= c;
					end if;
				when c=>
					if cntV <515 then
						state <= c;
					else
						state <= d;
					end if;
				when d =>
					if cntV <525 then
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
				Vs <= '0';
				V_blanck	<= '0';
			when b =>
				Vs <=  '1';
				V_blanck	<= '0';
				when c =>
				Vs <= '1';
				V_blanck	<= '1';
				when d =>
				Vs <= '1';
				V_blanck	<= '0';
				end case;
	end process;

end rtl;
