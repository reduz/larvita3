
import glob

xpm_list = glob.glob("*.xpm");

incfile = open("xpm_inc.h","wb")
skin_h_file = open("xpm_skin.h","wb")
skin_cpp_file = open("xpm_skin.inc","wb")


for x in xpm_list:

  incfile.write("#include \"editor/icons/"+x+"\"\n");
  bitmap_str = ("bitmap_editor_"+x[:-4]).upper();
  skin_h_file.write("\t"+bitmap_str+",\n");
  skin_cpp_file.write("SET_BITMAP( "+bitmap_str+",painter->load_xpm((const char**)"+x.replace(".","_")+"));\n");
  
  
  