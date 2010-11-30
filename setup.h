
Processing* processing;
Root* root;
string<uint8_t> processing_name;

void setup(void)
{
	root = new Root():

	processing = new Processing(root)
	processing_name = string<uint8_t>.from_str("proc");
	root->add_child(processing, processing_name);
}