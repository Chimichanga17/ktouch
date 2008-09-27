#!/usr/bin/perl -w --strict
use encoding utf8;
binmode STDOUT, ":utf8";
binmode STDIN, ":utf8";

# default values
$length_of_line = 40;
$number_of_line = 10;
$title = "Put your title here";

# genword( accumulated, core )
# This function will generate a random sequens of character
# containing only characters from accum or core and all words
# generated will contain at lest on character from core
sub genword
{
    (my $accum, my $core) = @_;

    my $all=$accum.$core;
    my $res="";

    for(my $i=0;$i<=rand(5);$i++)
    {
	my $ran_pos=int(rand(length($all)));
	$res=$res.substr($all,$ran_pos,1);
    }

    # check if we have generated a walid word, containig some characters from $core
    if ($res =~ m/[$core]/)
    {
	return $res;
    }
    else
    {
	return genword($accum,$core);
    }
}

# This function will return a list of words containing only characters from
# accum or core. And all words will contain at lest on echaracter from core.
# If we can't find enough words, we will use genword to generate enough words
# to fill the list.
sub genlist
{
    ($accum, $core) = @_;
    my @res;
 
    my $all=$accum.$core;


    #print "$core\n";

    foreach(@word_list)
    {
	chomp($_);
	if (m/[$core]/ && m/^[($all)][$all]*$/)
	{
	    push @res,$_;
	}
    }

    for(my $i=@res;$i<30;$i++)
    {
	push @res,genword($accum,$core);
    }

    return @res;
}


# Genlevel will generate a level
sub genlevel
{
    ($accum, $core) = @_;
    my @res = ();
    my @list = genlist($accum,$core);
    my $max_lines = $number_of_line;
    my $max_length = $length_of_line;
    while($max_lines >0)
    {
	my $line = "";
	my $tmp=$list[rand(@list)-1];
	$line = $line.$tmp; # first word on line should not have space 
	while($max_length >0)
	{
	    my $tmp=$list[rand(@list)-1];
	    $line = $line." ".$tmp; 
	    $max_length = $max_length - (length($tmp) + 1); # +1 is for counting one extra space for each word
	}
	$max_length = $length_of_line;
	$max_lines = $max_lines - 1;
	
	push @res, $line;
    }	
    return @res;
}

sub rrr
{
    print ".";
    s/\\/\\\\/g; #remove escape character...
    s/-/\\-/g; #remove any - since this will mean range
    s/ //g;
    return $_;
}

sub heading
{
    return
	"<KTouchLecture>\n".
	"    <Title>".$title."</Title>\n".
	"    <Comment>KTouch training file generated ".localtime(time())."\n".
	"Perl Script written by Steinar Theigmann &amp; Håvard Frøiland.</Comment>\n\n";
}

sub usage
{
    return
	"\n".
	"usage: ktouch-gen config_file\n".
	"   Example: ./ktouch-gen english-conf < english-word-file\n".
	"\n";
}

# --------------------- START ----------------------------

if(@ARGV == 0) # exit if there is no config_file specified
{
    die usage;
}

open(CONFIG,$ARGV[0]) # First argument should be config file
    or die "\ nCan't find config_file: $ARGV[0]\n\n";

# Read in all elements in config file
while (<CONFIG>)
{
    chomp($_);

    if(s/length\-of\-line//)
    {
	$length_of_line = $_;
    }
    elsif(s/number\-of\-line//)
    {
	$number_of_line = $_;
    }
    elsif(s/title//)
    {
	$title = $_;
    }
    elsif($_) # Add to config if not empty
    {
	push @config,$_;
    }
}

#foreach(@config)
#{
#    print "$_\n";
#}
#exit 0;

# Read in all words
while (<STDIN>)
{
    chomp($_);
    if(length($_)>0)
    {
	push @word_list, $_;
    }
}

print heading;

$accum="";

foreach(@config)
{
    print "    <Level>\n";
    print "        <NewCharacters>$_</NewCharacters>\n";
    foreach(genlevel($accum,$_)){
	print "        <Line>".$_."</Line>\n";
    }
    $accum=$accum.$_;
    print "    </Level>\n\n";
}


print "</KTouchLecture>\n";
