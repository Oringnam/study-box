import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import javax.swing.*;
import java.util.*;

public class TimeLinePanel extends JPanel {
	private ArrayList<FoodButtonPanel> list;
	
	public TimeLinePanel(){
		list = new ArrayList<FoodButtonPanel>();
		this.setPreferredSize(new Dimension(360, 130));
		this.setBackground(Color.black);
		this.setLayout(new GridLayout(1,3));
	}//TimeLinePanel()
	
	public void addF(FoodButtonPanel b){
		list.add(b);
		this.add(b);
		validate();
	}//addF()
	
	public void deleteF(FoodButtonPanel b){
		this.remove(b);
		list.remove(b);
		validate();	
	}//deleteF()
	
	public ArrayList<FoodButtonPanel>  getList(){
		return list;
	}//getList()
}//TimeLinePanel
