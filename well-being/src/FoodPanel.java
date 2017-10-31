import java.awt.*;
import javax.swing.*;

public class FoodPanel extends JPanel {

	private FoodButtonList list;
	private FoodButton[] button;
	private int target;
	private int i;
	private SecondPanel second;

	FoodPanel(SecondPanel second) {
		button = new FoodButton[Constant.FOODA.length];
		this.second = second;
		setBackground(Color.BLUE);
		setLayout(null);
		list = new FoodButtonList(this);
		GridLayout l = new GridLayout(10, 2);

		l.setHgap(2);
		l.setVgap(2);
		this.setLayout(l);		
		
		for(i=0; i<Constant.FOODA.length; i++) {
			
			button[i] = new FoodButton(new ImageIcon(Constant.FOODIMAGE[i]));
			button[i].setImageIconPath(Constant.FOODIMAGE[i]);
			button[i].setName(Constant.FOODA[i]);
			button[i].setCalories(Constant.foodData[i][0]);
			button[i].setTansoo(Constant.foodData[i][1]);
			button[i].setProtein(Constant.foodData[i][2]);
			button[i].setFat(Constant.foodData[i][3]);
			foodAdd(button[i]);
		}
	}

	public void listenerAdd() {
		for (FoodButton btn : list.getList()) {
			btn.addMouseListener(second.getMoveFoodListener());
		}
	}

	public void foodAdd(FoodButton x) {
		list.addB(x);
		x.addMouseListener(second.getMoveFoodListener());
	}
	public void foodDelete(FoodButton x) {
		list.deleteF(x);
	}
	public FoodButtonList getFoodButtonList() {
		return list;
	}
}