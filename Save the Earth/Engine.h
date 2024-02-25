#pragma once

#ifdef ENGINE_EXPORTS 
#define ENGINE_API _declspec(dllexport)
#else
#define ENGINE_API _declspec(dllimport)
#endif

constexpr float scr_width = 616.0f; //ACTIVE SCREEN 600 x 500, CELL 100 x 100, ROWS = 4, COLS = 6;
constexpr float scr_height = 539.0f;

constexpr int DL_OK = 3001;
constexpr int DL_FAIL = 3002;
constexpr int DL_NOT_SUPPORTED = 3003;

enum class dirs { stop = 0, left = 1, right = 2, up = 3, down = 4 };
enum class types {
	no_type = 0, hero = 1, monster1 = 2, monster2 = 3, monster3 = 4,
	bullet = 5, gold = 6, cloud1 = 7, cloud2 = 8, brick = 9, field = 10
};

struct ENGINE_API CELL
{
	int x = 0;
	int y = 0;
	int ex = 0;
	int ey = 0;
	int row = -1;
	int col = -1;
	int number = -1;
	types type = types::no_type;
};

class ENGINE_API OBJECT
{
	public:
		float x = 0;
		float y = 0;
		float ex = 0;
		float ey = 0;

		virtual ~OBJECT() {};

		void SetEdges()
		{
			ex = x + width;
			ey = y + height;
		}

		void NewDims(float _width, float _height)
		{
			width = _width;
			height = _height;
			ex = x + width;
			ey = y + height;
		}

		virtual void Release()
		{
			delete this;
		}
		
		static OBJECT* GetObject(float _where_x, float _where_y, float _awidth, float _height);
	
	protected:
		float width = 0;
		float height = 0;

		OBJECT(float _x, float _y, float _width, float _height)
		{
			x = _x;
			y = _y;
			width = _width;
			height = _height;
			ex = x + width;
			ey = y + height;
		}
};

class ENGINE_API DLINTERFACE :public OBJECT
{
	protected:
		float speed = 0.1f;
		
		int frame_delay = 10;
		int frame = 0;

		int killed_timer = 100;

		float max_jump_point = 0;
		bool max_jump_point_set = false;

		CELL my_cell;

	public:
		types type = types::no_type;
		dirs dir = dirs::stop;
		dirs jump_dir = dirs::stop;
		int lifes = 100;
		int screen = 0;

		bool alive = true;
		bool now_jumping = false;

		DLINTERFACE(float _x, float _y, float _width, float _height, types _type) :OBJECT(_x, _y, _width, _height)
		{
			type = _type;
			my_cell.row = static_cast<int>(y / 100);
			my_cell.col = screen * 6 + static_cast<int>(x / 100);
			my_cell.x = my_cell.col * 100;
			my_cell.y = my_cell.row * 100;
			my_cell.ex = my_cell.x + 99;
			my_cell.ey = my_cell.y + 99;
			my_cell.number = my_cell.row * 24 + my_cell.col;
		}
		virtual ~DLINTERFACE() {};

		int GetFrame()
		{
			frame_delay--;
			if (frame_delay < 0)
			{
				frame_delay = 10;
				frame++;
				if (frame > 5)frame = 0;
			}
			return frame;
		}
		int GetCol() const
		{
			return my_cell.col;
		}
		int GetRow() const
		{
			return my_cell.row;
		}
		int GetCell() const
		{
			return my_cell.number;
		}

		bool Killed()
		{
			killed_timer--;
			if (killed_timer <= 0)return true;
			return false;
		}

		void SetCellDims()
		{
			my_cell.col = screen * 6 + static_cast<int>(x / 100);
			my_cell.row = static_cast<int>(y / 100);
			my_cell.x = my_cell.col * 100;
			my_cell.y = my_cell.row * 100;
			my_cell.ex = my_cell.x + 99;
			my_cell.ey = my_cell.y + 99;
			my_cell.number = my_cell.row * 24 + my_cell.col;
		}

		void Fall(float gear = 1.0)
		{
			gear *= speed;
			if (ey + gear <= scr_height)
			{
				y += gear;
				SetEdges();
				SetCellDims();
			}
		}

		virtual void Release() = 0;
		virtual int Move(float gear = 1.0f) = 0;
		virtual int Jump(float gear = 1.0f) = 0;
};

typedef OBJECT* obj_ptr;
typedef DLINTERFACE* act_ptr;

extern ENGINE_API void CreateGrid(int first_x, int first_y, CELL(&Grid)[24][4]);
extern ENGINE_API act_ptr Factory(float first_x, float first_y, types _type);