#!/usr/bin/perl

use strict;
use warnings;

my $tag_file = "vendor/myhtml/include/myhtml/tag_const.h";
my $attribute_key_file = "scripts/attributes.gperf";

my $PACKAGE_NAME = "com.github.foobar27.myhtml4j.atoms";

my @forbidden_tag_list = qw/_UNDEF _TEXT _COMMENT _DOCTYPE _END_OF_FILE FIRST_ENTRY LAST_ENTRY/;
my $java_atom_directory = "../java/src/main/java/com/github/foobar27/myhtml4j/atoms";

my $java_tag_class_name = "Tags";
my $java_tag_class_file = "$java_atom_directory/$java_tag_class_name.java";

my $java_attribute_key_class_name = "AttributeKeys";
my $java_attribute_key_class_file = "$java_atom_directory/$java_attribute_key_class_name.java";

my $java_namespace_class_name = "Namespaces";
my $java_namespace_class_file = "$java_atom_directory/$java_namespace_class_name.java";

sub preprocess_names {
    my (%id2name) = @_;
    my $max_id = 0;
    while (my ($id, $name) = each(%id2name)) {
	if ($id > $max_id) {
	    $max_id = $id;
	}
    }
    return 
    { "id2name" => \%id2name,
      "max_id" => $max_id
    };
}

sub java_name {
    my ($input) = @_;
    if (!defined $input) {
	return $input;
    }
    $input = uc $input;
    $input =~ s/-/_/g;
    return $input;
}

sub generate_java_atom_class {
    my ($class_name, $filename, $tag_info, $atom_class) = @_;
    my $id2name = $tag_info->{"id2name"};
    my $max_id = $tag_info->{"max_id"};
    open(my $fh, '>', $filename) or die "Could not open file '$filename' $!";
    print $fh "// WARNING This file has been automatically generated, do not edit!\n";
    print $fh "package $PACKAGE_NAME;\n";
    print $fh "public class $class_name {\n";
    
    for (my $id=0; $id<=$max_id; ++$id) {
	my $name = $id2name->{$id};
	my $java_name = &java_name($name);
	if (defined $name) {
	    print $fh "    public static final $atom_class $java_name = new $atom_class($id, \"$name\");\n";
	}
    }
    print $fh "    public static final $atom_class" . "[] ALL_ATOMS = {\n";
    for (my $id=0; $id<=$max_id; ++$id) {
	my $name = $id2name->{$id};
	my $java_name = &java_name($name);
	if (defined $name) {
	    print $fh "        $java_name";
	} else {
	    print $fh "        null";
	}
    if ($id != $max_id) {
	print $fh ",";
	}
	print $fh "\n";
    }
    print $fh "    };\n";
    print $fh "}\n";
    close $fh;
}

sub load_tag_ids {
    my ($tag_file) = @_;
    my %forbidden_tag_set;
    @forbidden_tag_set{@forbidden_tag_list} = ();

    die "unknown file: '$tag_file' (did you init/update the submodules?)" unless -e $tag_file;

    open (my $fh, '<:encoding(UTF-8)', $tag_file)
	or die "Could not open file '$tag_file': $!";

    my %tags;
    while (<$fh>) {
	chomp;
	if (/MyHTML_TAG_(\S*)\s*= 0x(.*)/) {
	    my $name = $1;
	    my $id = $2;
	    $id =~ s/,$//;
	    $id = hex($id);
	    if (!exists $forbidden_tag_set{$name}) {
		$tags{$id} = $name;
	    }
	}
    }
    return %tags;
}

sub load_attribute_keys {
    my ($file) = @_;
    open (my $fh, "<:encoding(UTF-8)", $file)
	or die "Could not open file '$file': $!";
    my %tags;
    my $active = 0;
    while (<$fh>) {
	chomp;
	if (/%%/) {
	    $active = 1;
	} else {
	    if ($active) {
		if (/(.*),(\d+)/) {
		    my $name = $1;
		    my $id = $2;
		    $tags{$id} = $name;
		}
	    }
	}
    }
    return %tags;
}


my %namespaces = (
    0 => "undef",
    1 => "html",
    2 => "mathml",
    3 => "svg",
    4 => "xlink",
    5 => "xml",
    6 => "xmlns");

my $tag_info = &preprocess_names(&load_tag_ids($tag_file));
my $attribute_key_info = &preprocess_names(&load_attribute_keys($attribute_key_file));
my $namespace_info = &preprocess_names(%namespaces);
&generate_java_atom_class($java_tag_class_name, $java_tag_class_file, $tag_info, "Tag");
&generate_java_atom_class($java_attribute_key_class_name, $java_attribute_key_class_file, $attribute_key_info, "AttributeKey");
&generate_java_atom_class($java_namespace_class_name, $java_namespace_class_file, $namespace_info, "Namespace");
