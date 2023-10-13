#!/usr/local/bin/perl5.8.8

use strict;

&main();
exit 0;

sub main()
{
	my $eucjp = q{    # EUC-JPエンコーディングの構成文字
		[\x00-\x7F]    # ASCII/JISローマ字（1バイト）
		| \x8E[\xA0^\xDF]    # 半角カタカナ（2バイト）
		| \x8F[\xA1-\xFE][\xA1-\xFE]    # JIS X 2012-1990（3バイト）
		| [\xA1-\xFE][\xA1-\xFE]    # JIS X 0208-1997（2バイト）
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
	print "\t合計：$point\n";
}
