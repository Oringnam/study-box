import java.awt.*;
import javax.swing.*;
import javax.swing.border.Border;

import java.awt.event.*;


public class FirstPanel extends JPanel {
	public JPanel titlePanel;
	public JLabel title;
	public JTextField [] field;
	public JLabel [] lbl; 
	private JRadioButton rb1, rb2;
	private ImageIcon man,woman;
	private ButtonGroup group;
	int num;
	private FirstPanelListener listener=new FirstPanelListener();
	private JButton bbtn, nbtn;

	
	FirstPanel(){
		Color n = Constant.backColor;
		this.setBounds(0,0,800,1000);
		this.setLayout(null);
		this.setBackground(n);
		
		num = 4;
		field = new JTextField[num];
		lbl = new JLabel[num];
		titlePanel = new JPanel();
		title = new JLabel();
		man=new ImageIcon("picture/first/man.png");
		woman=new ImageIcon("picture/first/woman.png");
		rb1=new JRadioButton("����");
		rb1.addActionListener(listener);
		rb2=new JRadioButton("����");
		rb2.addActionListener(listener);
		group=new ButtonGroup();//��ư�� �׷�ȭ�Ͽ� �׷�ȭ�� ��ư�� �ϳ��� ���ð����ϵ��� ����
		
		titlePanel.setBounds(0,85,800,120);
		titlePanel.setBackground(Constant.c5);
		titlePanel.setLayout(null);
		add(titlePanel);
		
		//����
		title.setText("����� �����ΰ���?");
		title.setFont(new Font("���� ����� 230",Font.BOLD,60));
		title.setBounds(110, 0, 590, 120);
		title.setForeground(Constant.backColor);
		title.setHorizontalAlignment(SwingConstants.CENTER);
		title.setVerticalAlignment(SwingConstants.CENTER);
		titlePanel.add(title);
		
		
		
		
		//���� �׸�
		JLabel image1=new JLabel(man);
		image1.setBounds(175, 260, 200, 250);
		add(image1);
		
		//���� �׸�
		JLabel image2=new JLabel(woman);
		image2.setBounds(425,260,200,250);
		add(image2);
		
		//�������ù�ư ����
		rb1.setFont(new Font("���� ����� 230",20,20));
		rb1.setBounds(235, 480, 80, 50);
		rb1.setBackground(n);
		add(rb1);
		
		//�������ù�ư ����
		rb2.setFont(new Font("���� ����� 230",20,20));
		rb2.setBounds(485, 480, 80, 50);
		rb2.setBackground(n);
		add(rb2);
		
		//��ư�� �� �� �ϳ��� ������ �����ϵ��� �׷�ȭ
		group.add(rb1);
		group.add(rb2);
		
		//����ǥ�� ��
		lbl[0] = new JLabel("Age :");
		lbl[0].setFont(new Font("Segoe Print",Font.BOLD,25));
		lbl[0].setBounds(230, 590, 100, 50);
		add(lbl[0]);
		
		//���̸� �Է¹޴� �ؽ�Ʈ�ʵ�
		field[0] = new JTextField();
		field[0].setBounds(365, 590, 200, 50);
		field[0].setFont(new Font("���� ����� 230",Font.PLAIN,20));
		add(field[0]);
		
		//������ǥ�� ��
		lbl[1] = new JLabel("Weight :");
		lbl[1].setFont(new Font("Segoe Print",Font.BOLD,25));
		lbl[1].setBounds(230, 660, 150, 50);
		add(lbl[1]);
		
		//�����Ը� �Է¹޴� �ؽ�Ʈ�ʵ�
		field[1] = new JTextField();
		field[1].setBounds(365, 660, 200, 50);
		field[1].setFont(new Font("���� ����� 230",Font.PLAIN,20));
		add(field[1]);
		
		//Űǥ�� ��
		lbl[2] = new JLabel("Height :");
		lbl[2].setFont(new Font("Segoe Print",Font.BOLD,25));
		lbl[2].setBounds(230, 730, 150, 50);
		add(lbl[2]);
		
		//Ű�� �Է¹޴� �ؽ�Ʈ�ʵ�
		field[2] = new JTextField();
		field[2].setBounds(365, 730, 200, 50);
		field[2].setFont(new Font("���� ����� 230",Font.PLAIN,20));
		add(field[2]);
		
		//����ȭ������ ���ư��� ��ư
		bbtn= new JButton(new ImageIcon("picture/first/back.png"));
		bbtn.addActionListener(listener);
		bbtn.setBounds(670, 900, 50, 50);
		bbtn.setBorder(new RoundedBorder(70));
		bbtn.setBackground(n);
		bbtn.setForeground(n);
		add(bbtn);
		bbtn.setEnabled(false);//�� ȭ������ ���ư� �� ������ ��Ȱ��ȭ
		
		//����ȭ������ �Ѿư��� ��ư
		nbtn = new JButton(new ImageIcon("picture/first/next.png"));
		nbtn.addActionListener(listener);
		nbtn.setBounds(720, 900, 50, 50);
		nbtn.setBorder(new RoundedBorder(70));
		nbtn.setBackground(n);
		nbtn.setForeground(n);
		add(nbtn);
		
		
	}//FirstPanel()
	
	class RoundedBorder implements Border { //��ư�� �ٿ������ ���׶��� ����� ���� Ŭ����
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
	}//RoundedBorder


	
	private class FirstPanelListener implements ActionListener
	{//ù ������ ���� ������
		public void actionPerformed(ActionEvent e){

			if(e.getSource()==rb1)
			{				
				PrivateInfo.setSex(1);//1==����						
			}
			else if(e.getSource()==rb2)
			{
				PrivateInfo.setSex(2);//2==����				
			}
			else if(e.getSource()==nbtn)//���� ��ư���� �Ѿ�� ��ư�� Ŭ���� ��Ȳ����
			{
				if(PrivateInfo.getSex()==0)//���� �̼��ý� ����ó��
					{JOptionPane.showMessageDialog(FirstPanel.this, "Error : Select Gender.","Error",JOptionPane.WARNING_MESSAGE);}

				else {
					try{//�ؽ�Ʈ �ʵ忡 ���� ���ԷµǾ��� �ÿ� ����ó��
					
						Integer.parseInt(field[0].getText());
						Integer.parseInt(field[1].getText());
						Integer.parseInt(field[2].getText());
					
					}catch(IllegalArgumentException ectn){JOptionPane.showMessageDialog(FirstPanel.this, "Error : Fill All Blanks.","Error",JOptionPane.WARNING_MESSAGE);}
					
					if(PrivateInfo.getSex()!=0&&(Integer.parseInt(field[0].getText())>0)&&(Integer.parseInt(field[1].getText())>0)&&(Integer.parseInt(field[2].getText())>0))
					{//�Էµ� ���� ���� ������ ���Ͽ� ����ó��
						if(Integer.parseInt(field[0].getText())>=10&&Integer.parseInt(field[0].getText())<=100)
						{	
							PrivateInfo.setAge(Integer.parseInt(field[0].getText()));
							if(Integer.parseInt(field[1].getText())>=20&&Integer.parseInt(field[1].getText())<=110)
							{
								PrivateInfo.setWeight(Integer.parseInt(field[1].getText()));
								if(Integer.parseInt(field[2].getText())>=60&&Integer.parseInt(field[2].getText())<=220)
								{
									PrivateInfo.setHeight(Integer.parseInt(field[2].getText()));
									
									PrivateInfo.recommend();
									
									//���������� �ѱ��
									ViewControl.viewPanel(1, 2);
								}
								else
									JOptionPane.showMessageDialog(FirstPanel.this, "HEIGHT : 60 ~ 220.","Error",JOptionPane.WARNING_MESSAGE);
							}
							else
								JOptionPane.showMessageDialog(FirstPanel.this, "WEIGHT : 20 ~ 110","Error",JOptionPane.WARNING_MESSAGE);
							
						}
						else
							JOptionPane.showMessageDialog(FirstPanel.this, "AGE : 10 ~ 100","Error",JOptionPane.WARNING_MESSAGE);
						
					}
					else
						JOptionPane.showMessageDialog(FirstPanel.this, "Error : Wrong Number. (Need Proper Number) ","Error",JOptionPane.WARNING_MESSAGE);
				}
			}
		}
	}
}
