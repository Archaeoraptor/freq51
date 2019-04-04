function f=zabo(D,M)
tic
u=0.11688;
thedab=3*pi/180;
thedab1=0.045345;
thedab2=0.06413;
theda=-0.5:1/511:0.5;
n=length(theda);
G=zeros(1,n);
for i=1:n
    if (theda(i)>(-3*u)) && (theda(i)<(-2*u))
        G(i)=0.00316*exp(-2*log(2)*((theda(i)+2.5*u).^2)/thedab2.^2);
    elseif (theda(i)>(-2*u)) && (theda(i)<(-u))
        G(i)=0.01*exp(-2*log(2)*((theda(i)+1.5*u).^2)/thedab1.^2);
    elseif (theda(i)>(-u)) && (theda(i)<u)
        G(i)=exp(-2*log(2)*(theda(i).^2)/(thedab.^2));
    elseif (theda(i)>u) && (theda(i)<(2*u))
         G(i)=0.01*exp(-2*log(2)*((theda(i)-1.5*u).^2)/thedab1.^2);
    elseif (theda(i)>(2*u)) && (theda(i)<(3*u))
        G(i)=0.00316*exp(-2*log(2)*((theda(i)-2.5*u).^2)/thedab2.^2);
    else
        G(i)=0;
    end
end
%tianxian
lambda=0.02;
theta_a=3*pi/180; 
V=100;
fr=6000;
R=5000;
Rmax=30000;
H=2500;
thetap=60*pi/180; 
phi0=30*pi/180; 
R_cell=150;
theda=0:(45.8*pi/180)/511:(45.8*pi/180);
Aom=raylrnd(10,512,128); %瑞利分布 
Ac=25*mean(mean(Aom));
Cr=zeros(D,M);
for d=1:D
    r(d)=Rmax-(d-1)*R_cell; 
    phi(d)=asin(H/r(d)); 
     
vr=zeros(512,128);
for i=1:D
    fd(i)=2*V/lambda*cos(theda(i))*cos(phi(d));
    for m=1:M
        %********生成杂波谱为高斯谱******
        sigma_v2=V/(2*sqrt(2*log(2)))*theta_a; 
        sigma_v=sigma_v2; 
        sigma_f=2*sigma_v/lambda; 
        delta_f=0.6*(2*sqrt(2*log(2))*sigma_f); 
        sk=delta_f*1/sqrt(2*pi*sigma_f^2)*exp(-(([1:5]-3)*delta_f).^2/(2*sigma_f^2)); 
        kesai=exp(1i*2*pi*rand(1,5)); 
        xk=kesai.*sqrt(sk); 
        for k=1:M
            X(k)=sum(xk.*exp(1i*2*pi*([1:5]-3)*delta_f*k/fr)); 
        end  
        al=raylrnd(1.5,1,128); % 瑞利分布幅度
        vr(i,m)=(Ac+Aom(i,m))* al(m)*G(i).^2*X(m)*exp(1i*2*pi*(m-1)*fd(i)/fr)/r(d).^2;
    end
end

for m=1:M
    for n=1:512
    Cr(d,m)=Cr(d,m)+vr(n,m);
    end
end
end
f=Cr;
toc