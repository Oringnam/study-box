import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.*;
import java.util.*;

public class FoodButtonList 
{
	private FoodPanel p;
	private ArrayList<FoodButton> list;
	private int num;
	private static FoodButtonList me;
	
	// get
	public int getNum()						{ return num;			}
	public FoodButton getB(int a)			{ return list.get(a);	}	
	public ArrayList<FoodButton> getList()	{ return list;			}
	public static FoodButtonList getFoodButtonList(){ return me;}
	public FoodButtonList(FoodPanel x)
	{
		p = x;
		me = this; 
		list = new ArrayList<FoodButton>();
		num = -1;		
	}	
	public void addB(FoodButton k)
	{
		p.add(k);
		list.add(k);
		num++;
	}
	public void deleteF(FoodButton item){
		item.setVisible(false);
		p.remove(item);
		list.remove(item);		
		num--;
	}
}
