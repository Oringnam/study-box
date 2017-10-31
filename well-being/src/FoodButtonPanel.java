import java.awt.*;
import javax.swing.*;

public class FoodButtonPanel extends JPanel{
	private double fat;
	private double calories;
	private double protein;
	private double tansoo;
	private String name;
	private ImageIcon imageIcon;
	private String imageIconPath;
	private Image image;
	private int key;
	public  FoodButtonPanel(FoodButtonPanel f) {
		fat = f.fat;
		calories = f.calories;
		protein = f.protein;
		tansoo = f.tansoo;
		name = f.name;
		key =f.key;
		imageIconPath = f.imageIconPath;
		imageIcon = new ImageIcon(imageIconPath);
		image = imageIcon.getImage();
		
	}
	
	// get
	public double getFat()			{ return fat;		}
	public double getCalories()	{ return calories;	}
	public double getProtein()		{ return protein;	}
	public double getTansoo()		{ return tansoo;	}
	public String getName()		{ return name;		}
	
	// set
	public void setFat(double fat)				{ this.fat = fat;			}
	public void setCalories(double calories)	{ this.calories = calories; }
	public void setProtein(double protein)		{ this.protein = protein;	}
	public void setTansoo(double tansoo)		{ this.tansoo = tansoo;		}
	public void setName(String name)		{ this.name = name;			}

	public FoodButtonPanel() {
		super();
		this.setPreferredSize(new Dimension(185, 50));
	}

	public FoodButtonPanel (FoodButton f ) {
		
		
		fat = f.getFat();
		calories = f.getCalories();
		protein = f.getProtein();
		tansoo = f.getTansoo();
		name = f.getName();
		key = f.getKey();
		imageIconPath = f.getImageIconPath();
		imageIcon = new ImageIcon(imageIconPath);
		image = imageIcon.getImage();
		
	}
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		g.drawImage(image, 0, 0, getWidth(), getHeight(),this );
		
	}
}