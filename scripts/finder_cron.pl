#!/usr/bin/perl

my $finder_app = "/home1/strizhov/NetSec/zonecop/trunk/Finder/finder";
my $finder_config = "/home1/strizhov/NetSec/zonecop/trunk/Finder/finder.conf.sample";

# get timestamp
my ($sec, $min, $hr, $day, $mon, $year) = localtime();
$year = sprintf("%02d", $year + 1900);
$mon = sprintf("%02d", $mon+1);
$day = sprintf("%02d", $day);
my $timestamp = $year.$mon.$day;


system("$finder_app -c $finder_config");
if ( $? == -1 )
{
  print "Finder execution failed: $!\n";
}
else
{
  my $outfile = getOutFile($finder_config);
  my $stable = cutFileName($outfile);
  system("/bin/cp $outfile $stable");
  my $stable_timestamp = $stable.".".$timestamp;
  system("/bin/cp $stable $stable_timestamp");
}



sub getOutFile
{
 my $in=$_[0];
 my $out;
 open (config_in, $in) || die "couldn't open the $in file!"; 
 while(<config_in>)
 {
   my $line = $_;
   chomp($line);
   ($command, $eq, $param) = split (/ /, $line);
   if ($command eq "out_file")
   {
     $out = $param;
   }
 }
 return $out;
}


sub cutFileName
{
  my $in=$_[0];
  my $work = "work";
  ($dir, $work) = split(/$work/ ,$in);
  
  my $out = $dir."stable";
  return $out;
}

