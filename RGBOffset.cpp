

static const char * const HELP = "RGBOffset";
static const char * const CLASS = "RGBOffset";


#include "DDImage\Iop.h"
#include "DDImage\Row.h"
#include "DDImage\Knobs.h"
#include "DDImage\Tile.h"
#include "DDImage\DDMath.h"
//导入需要的头文件


using namespace DD::Image;


class RGBOffset : public Iop {

public:

	int r_offset[2];
	int g_offset[2];
	int b_offset[2];

	int offset_max;

	RGBOffset(Node * node) :Iop(node) {
		r_offset[0] = r_offset[1] = g_offset[0] = g_offset[1] = b_offset[0] = b_offset[1] = 0;
		offset_max = 0;
	
	}


	void _validate(bool for_real);
	void _request(int x, int y, int r, int t, ChannelMask channels, int count);
	void engine(int y, int l, int r, ChannelMask channels, Row& row);


	virtual void knobs(Knob_Callback);
	const char* Class() const { return CLASS; }
	const char* node_help() const { return HELP; }
	static const Iop::Description d;

};




void RGBOffset::_validate(bool for_real) {

	offset_max = MAX(MAX(MAX(abs(r_offset[0]), abs(r_offset[1])), MAX(abs(g_offset[0]), abs(g_offset[1]))), MAX(abs(b_offset[0]), abs(b_offset[1])));
	//找出边界框最大值
	copy_info();
	info_.pad(offset_max);

};


void RGBOffset::_request(int x, int y, int r, int t, ChannelMask channels, int count) {

	
	input(0)->request(x - offset_max, y - offset_max, r + offset_max, t + offset_max, channels, count);

};


void RGBOffset::engine(int y, int x, int r, ChannelMask channels, Row& row) {

	
	if (!offset_max) { offset_max = 1; };
	Tile tile(input0(), x - offset_max, y - offset_max, r + offset_max, y + offset_max, channels);//设置tile缓存
	if (aborted()) {
		std::cerr << "Aborted!";
		return;//tile缓存设置失败时直接return
	}



	foreach (z,channels)//循环传入通道
	{
		
		
		int x_offset = 0;
		int y_offset = 0;

		
		//根据通道的不同设置偏移量
		if (z == Chan_Red) { x_offset = r_offset[0]; y_offset = r_offset[1]; };
		if (z == Chan_Green) { x_offset = g_offset[0]; y_offset = g_offset[1]; };
		if (z == Chan_Blue) { x_offset = b_offset[0]; y_offset = b_offset[1]; };



		float * out = row.writable(z) + x ;//设置输出指针开始的地址
		for (int i = x; i < r; i++) {
			//把输出像素值设置为偏移后位置的像素值
			out[i] = tile[z][tile.clampy(y + y_offset)][tile.clampx(i + x_offset)];

		};

	};


};






void RGBOffset::knobs(Knob_Callback f)
{	//设置旋钮
	MultiInt_knob(f,r_offset,2,"r_offset");
	Tooltip(f, "r offset");
	MultiInt_knob(f, g_offset, 2, "g_offset");
	Tooltip(f, "g offset");
	MultiInt_knob(f, b_offset, 2, "b_offset");
	Tooltip(f, "b offset");
}

static Iop* build(Node* node)
{
	return new RGBOffset(node);
}
const Iop::Description RGBOffset::d(CLASS, "RGBOffset", build);




