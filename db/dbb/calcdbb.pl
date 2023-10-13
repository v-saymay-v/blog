#!/usr/local/bin/perl5.8.8

use strict;

&main();
exit 0;

sub main()
{
	my $eucjp = q{    # EUC-JP���󥳡��ǥ��󥰤ι���ʸ��
		[\x00-\x7F]    # ASCII/JIS���޻���1�Х��ȡ�
		| \x8E[\xA0^\xDF]    # Ⱦ�ѥ������ʡ�2�Х��ȡ�
		| \x8F[\xA1-\xFE][\xA1-\xFE]    # JIS X 2012-1990��3�Х��ȡ�
		| [\xA1-\xFE][\xA1-\xFE]    # JIS X 0208-1997��2�Х��ȡ�
	};
	my $point = 0;
	my %temp;
	my %kind;
	while (<STDIN>) {
		if ($_ =~ /^ (?: $eucjp)*? \xCC\xEE\xB5\xE5/ox) {
			my @line = split(/\t/, $_);
			my $word = $line[0];
			chomp($line[3]);
			my @item = split(/-/, $line[3]);
			if (!$temp{$line[0]}) {
				$item[3] =~ s/M/-/;
				$temp{$line[0]} = $item[3];
				$point += $item[3];
				if ($kind{$item[2]}) {
					$kind{$item[2]} += $item[3];
				} else {
					$kind{$item[2]} = $item[3];
				}
				print $_;
			}
		}
	}
	print %kind;
	print "\t��ס�$point\n";
}
