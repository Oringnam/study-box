import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.Border;

public class ThirdPanel extends JPanel{
	//private JPanel upPanel, downPanel;
	private JButton bbtn,nbtn,btnThx;
	private JPanel imgPanel,recomPanel, titlePanel1, titlePanel2;
	private JLabel lbladv,lblImg;
	private int check;
	private ImageIcon icon;
	private Recommend rec;
	
	private ThirdPanelListener listener;
	private AdvicePanel tot,dan,tan,ji;
	Color n = Constant.backColor;
	
	public ThirdPanel(){
		
		this.setBounds(0,0,800,1000);
		this.setLayout(null);
		this.setBackground(n);
		
		// page change button
		bbtn= new JButton(new ImageIcon("picture/first/back.png"));
		nbtn = new JButton(new ImageIcon("picture/first/next.png"));
		btnThx = new JButton("Thx :)");
		listener = new ThirdPanelListener();		
		btnThx.addActionListener(listener);
		
		bbtn.addActionListener(listener);
		bbtn.setBounds(670, 900, 50, 50);
		bbtn.setBorder(new RoundedBorder(70));
		bbtn.setBackground(n);
		bbtn.setForeground(n);
		add(bbtn);		
		
		nbtn.addActionListener(listener);
		nbtn.setBounds(720, 900, 50, 50);
		nbtn.setBorder(new RoundedBorder(70));
		nbtn.setBackground(n);
		nbtn.setForeground(n);	
		nbtn.setEnabled(false);
		add(nbtn);	
		
		titlePanel1 = new JPanel();
		titlePanel1.setBounds(0,85,800,120);
		titlePanel1.setBackground(Constant.c5);
		add(titlePanel1);
		
		
		// advice title .. Top
		lbladv = new JLabel("Result! Check Please!");
		lbladv.setFont(new Font("Segoe Print",Font.BOLD,60));
		lbladv.setForeground(n);
		lbladv.setBounds(10,20,780,120);
		lbladv.setHorizontalAlignment(SwingConstants.CENTER);
		lbladv.setVerticalAlignment(SwingConstants.CENTER);
		titlePanel1.add(lbladv);		
		
		
	}	// ThirdPanel()
	
	public void recommend() {
		// background panel
		recomPanel = new JPanel();
		recomPanel.setBounds(0,600,800,400);
		recomPanel.setBackground(n);
		recomPanel.setLayout(null);
		add(recomPanel);
		rec = new Recommend();
		
		Font fnt = new Font("Segoe Print",Font.BOLD,20);		
		
		titlePanel2 = new JPanel();
		titlePanel2.setBounds(0,0,800,80);
		titlePanel2.setBackground(Constant.c5);
		recomPanel.add(titlePanel2);
		
		JLabel title = new JLabel("Advice!");
		title.setFont(new Font("Segoe Print",Font.BOLD,50));
		title.setForeground(n);
		title.setHorizontalAlignment(SwingConstants.CENTER);
		title.setVerticalAlignment(SwingConstants.CENTER);
		title.setBounds(0,10,800,40);
		titlePanel2.add(title);		
		
		rec.setBounds(40,100,900,300);
		recomPanel.add(rec);

		btnThx.setBounds(325,280,150,40);
		btnThx.setBackground(n);
		btnThx.setForeground(Constant.c5);
		btnThx.setFont(fnt);
		btnThx.setBorder(new RoundedBorder(25));
		recomPanel.add(btnThx);
	}
	
	class RoundedBorder implements Border { 
		int radius;
		RoundedBorder(int radius) { 
			this.radius = radius; 
		} 
		public Insets getBorderInsets(Component c) { 
			return new Insets(this.radius + 1, this.radius + 1, this.radius + 2, this.radius); 
		} 
		public boolean isBorderOpaque() { 
			return true; 
		} 
		public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) { 
			g.drawRoundRect(x, y, width - 1, height - 1, radius, radius); 
		} 
	}	// RoundedBorder()
	
	public void Analysis() {
		// main	.. Mid right
		// 1. Total
		tot = new AdvicePanel("칼로리",TotalAted.gettCal(),PrivateInfo.getRecomCal());
		tot.setBounds(75,250,315,150);
		add(tot);
		
		// 2. Protein
		dan = new AdvicePanel("단백질",TotalAted.gettProt(),PrivateInfo.getRecomProt());
		dan.setBounds(75,415,315,150);
		add(dan);
		
		// 3. Carbohydrate
		tan = new AdvicePanel("탄수화물",TotalAted.gettCarbo(),PrivateInfo.getRecomCarbo());
		tan.setBounds(410,250,315,150);
		add(tan);
		
		// 4. Fat
		ji = new AdvicePanel("지방",TotalAted.gettFat(),PrivateInfo.getRecomFat());
		ji.setBounds(410,415,315,150);
		add(ji);
	}	// Analysis()
	
	private class ThirdPanelListener implements ActionListener {		
		public void actionPerformed(ActionEvent e){
			Object obj = e.getSource();
			if(obj == bbtn) {
				ViewControl.viewPanel(3, 2);
				remove(tot);
				remove(tan);
				remove(ji);
				remove(dan);
			//	remove(titlePanel2);
				remove(recomPanel);
			} else if (obj == btnThx) {
				System.exit(1);				
			}	// if.. else..
		}		// actionPerformed()
	}	// ThirdPanelListner class
}	// ThirdPanel class
