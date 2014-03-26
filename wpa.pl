#!/usr/bin/perl
use strict;
use warnings;
use Net::Pcap; #Pcap driver module
use Net::Bluetooth;
use NetPacket::Ethernet;
use NetPacket::IP;

# use Wx; #GUI module

#Title: Wireless Proximity Analyzer
#Authors: Ross Swerling, Cal Pitner, Joe Sirianni
#License: Apache v2


#my @input = <STDIN>;
my $err = 0;
my %devinfo = ();


print "WiFi Devices:\n";
my @WiFiDevs = Net::Pcap::pcap_findalldevs(\%devinfo, \$err);
foreach (@WiFiDevs) {
	print "$_ : $devinfo{$_}\n";
}

#print "\nBluetooth Devices:\n";
#my $BTDevs_ptr = Net::Bluetooth::get_remote_devices(); 
#foreach my $addr (keys %$BTDevs_ptr) {
#	print "Address: $addr Name: $BTDevs_ptr->{$addr}\n";
#}

print "\nSelect a device: ";
my $devChoice = <STDIN>;
chomp ($devChoice);
print "Selected: $WiFiDevs[$devChoice - 1]\n";

my $pcap = Net::Pcap::pcap_open_live($WiFiDevs[$devChoice - 1], 14, 0, 0, \$err) or die "Can't open device : $err\n";

Net::Pcap::loop($pcap, -1, \&packetCapture, '') or die 'Unable to capture packets';
Net::Pcap::close($pcap);

sub packetCapture {
	my ($user_data, $header, $packet) = @_;
	my $ether = NetPacket::Ethernet->decode($packet);
	
	print "MAC: ", $ether->{'src_mac'}, "\n";
}
