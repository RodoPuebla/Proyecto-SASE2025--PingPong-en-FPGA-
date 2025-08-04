library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity contador_vertical is

	port
	(
		clk		: in std_logic;
		reset		: in std_logic;
		enable	: in std_logic;
		q			: out integer range 0 to 525;
		x			: out std_logic
	);

end entity;

architecture rtl of contador_vertical is
begin
process (clk,reset)
	variable   cnt		   : integer range 0 to 525;
begin
	if reset = '0' then
		cnt := 0;
	elsif rising_edge(clk) then
			if enable = '1' then
				 if cnt /= 525 then
					  cnt := cnt + 1;
					  x <= '0';
				 else
					  cnt := 0;
					  x <= '1';
				 end if;
			else
				 x <= '0';
			end if;
	  end if;
	q <= cnt;
end process;

end rtl;