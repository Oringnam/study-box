import java.awt.*;
import javax.swing.*;
import javax.swing.border.Border;
import java.awt.event.*;

public class StartPanel extends JPanel{
	private JButton 	btnWonder, btnNoWonder;	// 궁금하다 , 궁금하지 않다
	private JPanel		titlePanel, mainPanel, bottomPanel;
	private JLabel		lblTitle, lblPhoto, lblQuestion , lblCopyright;	// 제목, 사진, 저작권
	private ImageIcon	iconPhoto, iconQuestion;	// 메인화면 - 사진, 말풍선
	private ButtonListener btnL;
	
	public StartPanel(){		
		Color n= Constant.backColor;
		this.setBounds(0,0,800,1000);
		this.setLayout(null);
		this.setBackground(n);
		

		btnL = new ButtonListener();	
		titlePanel = new JPanel();
		titlePanel.setBounds(0,85,800,120);
		titlePanel.setBackground(Constant.c5);
		titlePanel.setLayout(null);
		add(titlePanel);
		
		lblTitle = new JLabel("Eating is important");
		lblTitle.setBounds(0,0,800,120);
		lblTitle.setFont(new Font("Segoe Print",Font.BOLD,60));
		lblTitle.setForeground(Constant.backColor);
		lblTitle.setHorizontalAlignment(SwingConstants.CENTER);
		lblTitle.setVerticalAlignment(SwingConstants.CENTER);
		titlePanel.add(lblTitle);	
		
		mainPanel = new JPanel();
		mainPanel.setBounds(0,110,800,700);
		mainPanel.setBackground(n);
		mainPanel.setLayout(null);
		add(mainPanel);				
		
		iconPhoto = new ImageIcon("picture/start/donguri.jpg");
		lblPhoto = new JLabel(iconPhoto);
		lblPhoto.setBounds(-154,88,750,780);
		mainPanel.add(lblPhoto);	
		
		iconQuestion = new ImageIcon("picture/start/q.png");
		lblQuestion = new JLabel(iconQuestion);
		lblQuestion.setBounds(390,80,400,400);
		mainPanel.add(lblQuestion);
		
		Font fntButton = new Font("Segoe Print",Font.PLAIN,20);	
		
	//	btnWonder = new JButton(new ImageIcon("picture/start/wNw.png"));
		btnWonder = new JButton("Wonder");
		btnWonder.setFont(fntButton);
		btnWonder.setForeground(Constant.c2);
		btnWonder.setBackground(n);
		btnWonder.setBounds(530,495,170,60);
		btnWonder.addActionListener(btnL);
		btnWonder.setBorder(new RoundedBorder(25));
		mainPanel.add(btnWonder);		
		
//		btnNoWonder = new JButton(new ImageIcon("picture/start/wNw.png"));
		btnNoWonder = new JButton("No Wonder");
		btnNoWonder.setFont(fntButton);
		btnNoWonder.setBackground(n);
		btnNoWonder.setBounds(530,585,170,60);
		btnNoWonder.addActionListener(btnL);
		btnNoWonder.setBorder(new RoundedBorder(25));
		mainPanel.add(btnNoWonder);	
		
		bottomPanel = new JPanel();
		bottomPanel.setBounds(0,880,800,50);
		bottomPanel.setBackground(Constant.c5);
		bottomPanel.setLayout(null);
		add(bottomPanel);
		
		lblCopyright = new JLabel("COPYRIGHTS TEAM-10 Wellbeing.  ALL RIGHTS RESERVED");
		lblCopyright.setFont(new Font("Verdana",Font.BOLD,15));
		lblCopyright.setForeground(Constant.backColor);
		lblCopyright.setHorizontalAlignment(SwingConstants.CENTER);
		lblCopyright.setVerticalAlignment(SwingConstants.CENTER);
		lblCopyright.setBounds(0,0,800,50);
		bottomPanel.add(lblCopyright);		
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
	}
	
	public void wannaOut() {
		int question = JOptionPane.showConfirmDialog(null,"Do you want to go out?");
		if(question == JOptionPane.YES_OPTION) {
			System.exit(1);	
		}
		else if(question == JOptionPane.NO_OPTION) {
			ViewControl.viewPanel(4,1);	
		}
		else {
			// do nothing
			// stay this page	
		}
	}	// wannaOut()
	private class ButtonListener implements ActionListener
	{
		public void actionPerformed(ActionEvent event)
		{
			Object obj = event.getSource();
			
			if(obj == btnWonder) {
				ViewControl.viewPanel(4,1);
			}
			else if(obj == btnNoWonder) {
				wannaOut();
			}
		}
	}
}
