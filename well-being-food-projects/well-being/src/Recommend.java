import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;

public class Recommend extends JPanel {
	private int loc;
	private boolean check;
	private JLabel lbl;
	private double prot, carbo, fat, min, least;
	private ArrayList<FoodButton> list;
	private Imgpnl imgpnl;

	public Recommend() {
		this.setPreferredSize(new Dimension(650, 150));
		this.setBackground(Constant.backColor);
		check = true;

		lbl = new JLabel();
		lbl.setFont(new Font("���� ����� 230", Font.PLAIN, 25));
		lbl.setHorizontalAlignment(SwingConstants.RIGHT);
		lbl.setVerticalAlignment(SwingConstants.CENTER);
		lbl.setBounds(0, 0, 650, 150);
		this.add(lbl);

		// ���� ������ ����Ʈ
		list = FoodButtonList.getFoodButtonList().getList();
		// standard
		prot = TotalAted.gettProt() - PrivateInfo.getRecomProt();
		carbo = TotalAted.gettCarbo() - PrivateInfo.getRecomCarbo();
		fat = TotalAted.gettFat() - PrivateInfo.getRecomFat();
		
		// min data init
		min =  (	(list.get(0).getProtein() - prot) * (list.get(0).getProtein() - prot) / PrivateInfo.getRecomProt()	/ PrivateInfo.getRecomProt()
				+ 	(list.get(0).getTansoo() - carbo) * (list.get(0).getTansoo() - carbo)	/ PrivateInfo.getRecomCarbo() / PrivateInfo.getRecomCarbo()
				+	(list.get(0).getFat() - fat) * (list.get(0).getFat() - fat) / PrivateInfo.getRecomFat()/ PrivateInfo.getRecomFat()	);

		// find min value
		for (int i = 1; i < list.size(); i++) {
			least = (	(list.get(i).getProtein() - prot) * (list.get(i).getProtein() - prot) / PrivateInfo.getRecomProt()	/ PrivateInfo.getRecomProt()
					+ 	(list.get(i).getTansoo() - carbo) * (list.get(i).getTansoo() - carbo)	/ PrivateInfo.getRecomCarbo() / PrivateInfo.getRecomCarbo()
					+	(list.get(i).getFat() - fat) * (list.get(i).getFat() - fat) / PrivateInfo.getRecomFat()/ PrivateInfo.getRecomFat()	);
			if (min > least) {
				loc = i;
				min = least;
			}
		}
		for (int i = 1; i < list.size(); i++) {
			least = (	(list.get(i).getProtein() + prot) * (list.get(i).getProtein() + prot) / PrivateInfo.getRecomProt()	/ PrivateInfo.getRecomProt()
					+ 	(list.get(i).getTansoo() + carbo) * (list.get(i).getTansoo() + carbo)	/ PrivateInfo.getRecomCarbo() / PrivateInfo.getRecomCarbo()
					+	(list.get(i).getFat() + fat) * (list.get(i).getFat() + fat) / PrivateInfo.getRecomFat()/ PrivateInfo.getRecomFat()	);
			if (min > least) {
				loc = i;
				min = least;
				check = false;
			}
		}
		
		// adding img and label
		imgpnl=new Imgpnl();
		lbl.add(imgpnl);
		this.printImg();
	} // Recommend()

	public void printImg() {
		if (check == true) { // abundance.
			lbl.setText("���� ������ ���ϴ°� ���ƿ�~");

		} else { // shortage
			lbl.setText("���� ������ ã�ƺ���~");
		} // if.. else
	}

	class Imgpnl extends JPanel {
		private ImageIcon imageIcon;
		private Image image;

		private Imgpnl() {
			setPreferredSize(new Dimension(300, 150));
			setBounds(0, 0, 300, 150);
			imageIcon = new ImageIcon(list.get(loc).getImageIconPath());
			image = imageIcon.getImage();
		}	// Imgpnl()

		public void paintComponent(Graphics g) {
			super.paintComponent(g);
			g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
		}	// paintComponent()
	}	// Imgpnl class
} // Recommend class
