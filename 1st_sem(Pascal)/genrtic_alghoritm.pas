program genesis;

  const l=16; //длина набора
        n=10; //количество наборов
        d=4; //длина диапазона
        stopcount=2000; //сколько раз нужно выполнить алгоритм для остановки
        sign=1; //знак. для переключеия на минимум/максимум
        
  type t=array[1..l] of 0..1;
       data=record
         info:t;  //сам набор
         res:real; //y(x)
         tr:real;  //x
         m:integer; //количество мутаций для этого набора
       end;
       mas=array[1..n] of data; //массив всех наборов
       
  var p:mas;
      i,j,a,b,mode,qc,pc,qm,pm,mx,count,answ,answ2,answ3,step,k,y:integer;
      tx:text;
      s1,s2:data;
      rez:real;
  
  procedure create(var a:mas); //начальная генерация значений
    var i,j:integer;
    begin
      for i:=1 to n do
        for j:=1 to l do
          a[i].info[j]:=random(2);
    end;
    
  procedure outputdata(str:data); //вывод набора
    var i:integer;
    begin
      for i:=1 to l do
        write(str.info[i]);
    end;
    
  procedure outputdatatxt(str:data); //вывод набора
    var i:integer;
    begin
      for i:=1 to l do
        write(tx,str.info[i]);
    end;
    
  procedure outputmas(p:mas);  //вывод всех данных
    var i:integer;
    begin
      for i:=1 to n do
        begin
          outputdata(p[i]);
          write(' x=',p[i].tr:6:6);
          write(' f(x)=',p[i].res:6:6);
          //write(' mut=',p[i].m);
          writeln;
        end;
      writeln;
    end;
    
  procedure outputmastxt(p:mas);  //вывод всех данных
    var i:integer;
    begin
      for i:=1 to n do
        begin
          outputdatatxt(p[i]);
          write(tx,' x=',p[i].tr:6:6);
          write(tx,' f(x)=',p[i].res:6:6);
          //write(tx,' mut=',p[i].m);
          writeln(tx);
        end;
      writeln(tx);
    end;
  
  procedure outputstr(t:string;num:integer);  //вывод конкретной группы данных. для сокращения кода
    begin
      if (mode=3) and (answ2=1) then
        begin
          writeln(tx,t);
          outputdatatxt(p[num]);
          if (answ3=1) then
            begin
              writeln(t);
              outputdata(p[num]); 
            end;
        end;
    end;
    
  function f(var x:real):real; //сама функция
    begin
      f:=(x-3)*(x-2)*(x-0.01)*(x-0.01)*(x-0.01)*(x-0.01)*(x-3.99)*(x-3.99)*(x-3.99)*(x-3.99)*(1-exp(x-1.5))*sin(x/3+0.2);
      //f:=x*(sqr(sqr(x-1.1)))*(x-1.1)*sqr(sqr(x-1.2))*sqr(x-1.3)*(x-1.3)*cos(x+100);
      //f:=x*sin(x+5)*cos(x-6)*sin(x+7)*cos(x-8)*sin(x/3);
    end;
    
  procedure recount(var p:mas); //получение по набору значений х и у
    var i,s,j:integer;
        trs:real;
        k:longint;
    begin
      for i:=1 to n do
        begin
          begin
            s:=0;
            k:=1;
            for j:=l downto 1 do //перевод в десятичную систему в целое число
            begin
              s:=s+(p[i].info[j])*k;
              k:=k*2;
            end;
            trs:=d*s/k;
          end;
          p[i].tr:=trs;
          p[i].res:=f(trs);
        end;
    end;
    
    procedure sort;  // сортировка массива наборов
    var i,j:integer;
        t:data;
    begin
      for i:=1 to n-1 do
        for j:=i+1 to n do
        if sign*p[i].res<sign*p[j].res then
            begin
              t:=p[j];
              p[j]:=p[i];
              p[i]:=t;
            end;
      end;
    
  function select(var p:mas):integer; //отбор особи для скрещивания
    var a,b,c,rc:integer;
    begin
      repeat
        a:=random(n)+1; //иначе можем получить 0
        b:=random(n)+1;
      until (a<>b);
      rc:=random(101);
      if (a<b) and (rc>=3) then c:=a  //дает слабому набору маленький шанс на победу
      else c:=b;
      select:=c;
    end;

  procedure crossbreeding(a,b:integer); //скрещивание двух особей
    var i,r,max:integer;
    begin
      y:=random(l)+1;
      s1:=p[a];
      s2:=p[b];
      if (a>b) then max:=a
        else
          max:=b;
      for i:=y to l do
        begin
          r:=s1.info[i];  //обмен битами начиная со случайного с-го
          s1.info[i]:=s2.info[i];
          s2.info[i]:=r;
        end;
      p[n]:=s1;  // замена старых наборов на новые: самый слабый набор (из всех) заменяется на резальтат скрещивания, а самый слабый из родителей-на другой результат скрещивания
      p[max]:=s2;
      sort;
      recount(p);
    end;
      
  procedure mutation; //мутация для случайного числа наборов
    var c,d,e,r,num,onecount,i:integer;
    begin
      num:=random(n-1)+1; // +2 для защиты максимального набора от мутации
        outputstr(' Original number',num); 
      c:=random(l)+1;
      d:=random(l)+1;
      onecount:=0;
      for i:=1 to l do
        onecount:=onecount+p[num].info[i];
      if (onecount>n-4) or (onecount<5) then  //зменение слишком однородного набора
        begin
          e:=random(l)+1;
          p[num].info[e]:=1-p[num].info[e];
        end;
        if (mode=3) and (answ2=1) then  //промежуточный вывод
          writeln(tx,' Bits: ',c,' ',d);
        if (mode=3) and (answ2=1) and (answ3=1) then
          writeln(' Bits: ',c,' ',d);
      r:=p[num].info[c];       
      p[num].info[c]:=p[num].info[d];
      p[num].info[d]:=r;
      p[num].m:=p[num].m+1;
      {p[num].info[c]:=1-p[num].info[c];
      p[num].info[d]:=1-p[num].info[d];}
        outputstr(' Final number',num);  
      sort;
      recount(p);
    end;

  {процедуры, необходимые только для упрощения вывода}

  procedure standardinput;  //ввод всех необходимых параметров. вынесен в процедуру для большей понятности основной программы
    begin
      writeln('Input program mode: 1-standard, 2-detailed,3-test'); //рекомендуемые параметры вариаций: 4 и 3, 100%
      readln(mode);
      writeln('Input crossbreeding quantity (qc/n)');
      readln(qc);
      writeln('Input crossbreeding probability(%)');
      readln(pc);
      writeln('Input mutation quantity (qm/n)');
      readln(qm);
      writeln('Input mutation probability(%)');
      readln(pm);
      randomize;
      create(p);
      recount(p);
      if(mode=3) then
        begin
          assign(tx,'F:\Genetic algorithms\tx.txt');
          rewrite(tx);
          writeln(tx,' Genetic algorithms');
          writeln(tx,'Crossbreeding quantity (qs/n): ',qc);
          writeln(tx,'Crossbreeding probability(%): ',pc);
          writeln(tx,'Mutation quantity (qm/n): ',qm);
          writeln(tx,'Mutation probability(%): ',pm);
          writeln(tx);
          writeln('Do you want to use standart count of steps? (',stopcount,'), 1-yes,2-no');
          readln(answ);
          if (answ=2) then
            begin
              writeln('Input steps count');
              readln(count);
            end
          else count:=stopcount;
          writeln('Do you want to write all information into file? 1-yes,2-no');
          readln(answ2);
          writeln('Do you want to write all information on screen? 1-yes,2-no');
          readln(answ3);
        end
        else count:=stopcount;
      if (mode=2) then
        begin
          writeln('Original set:');
          writeln;
          outputmas(p);
          if (mode=3) then
            begin
              writeln(tx,'Original set:');
              writeln(tx);
              outputmastxt(p);
            end;
        end;
  end;
  
  procedure steps;
    begin
      step:=step+1;
      if (mode=3) and (answ2=1) then
        begin
          writeln(tx);
          writeln(tx,' Step: ',step);
          outputmastxt(p);
          writeln(tx);
          writeln(tx,'Crossbreeding:');
        end;
      if (mode=3) and (answ2=1) and (answ3=1) then
        begin
          writeln;
          writeln(' Step: ',step);
          outputmas(p);
          writeln;
          writeln('Crossbreeding:');
        end;
    end;

  procedure cbnum(t:string);   //вывод двух элементов массива и строки (для вывода при скрещивании)
    begin
      if (mode=3) and (answ2=1) then
        begin
          writeln(tx);
          writeln(tx,t);
          outputdatatxt(s1);writeln(tx,' ',a);
          outputdatatxt(s2);write(tx,' ',b);writeln(tx);
          if (answ3=1) then
            begin
              writeln;
              writeln(t);
              outputdata(s1);writeln(' ',a);
              outputdata(s2);write(' ',b);writeln;
            end; 
        end;
    end;
  
  procedure no(t:string);
    begin
      if (mode=3) and (answ2=1) then
        begin
          writeln(tx);
          writeln(tx,'Var: ',mx);
          writeln(tx,t);
          if (answ3=1) then
            begin
              writeln;
              writeln('Var: ',mx);
              writeln(t);
            end;
        end;
    end;
  
begin
  answ2:=2;
  answ3:=2;
  standardinput;
  if (mode<>3) then
    begin
  writeln('Do you want to use standart count of steps? (',stopcount,'), 1-yes,2-no');
          readln(answ);
          if (answ=2) then
            begin
              writeln('Input steps count');
              readln(count);
            end
          else count:=stopcount;
   end;
  
  {основная часть}
  recount(p);
  sort;
  rez:=p[1].res;
  step:=0;
  for i:=1 to count do
    begin
      steps;
      for j:=1 to qc do // qc раз выбираем по 2 особи и скрещиваем их (выбираем 2 раза подряд, но выбор случайный
        begin
          recount(p);
          sort;
          repeat
            a:=select(p);
            b:=select(p);
          until(a<>b);
          mx:=random(101);  //выбираем случайное число. Если оно меньше вероятности скркщивания, то скрещивание происходит (как раз в pm% случаев), иначе-нет
          if(mx<=pc) then
            begin
              s1:=p[a];
              s2:=p[b];
              cbnum('Original numbers');
              crossbreeding(a,b);
              if (mode=3) and (answ2=1) then
                begin
                  writeln(tx,'Var: ',mx,' bite: ',y);
                  if (answ3=1) then
                    writeln('Var: ',mx,' bite: ',y);
                end;
              cbnum('Final numbers');
              //writeln(tx);
              recount(p);
              sort;
              if (mode=3) and (answ2=1) then
                writeln(tx);
            end;
          if (mx>pc) then
            begin
              cbnum('Original numbers');
              no('No crossbreeding');
            end;
      end;
      recount(p);
      sort;
        if (mode=3) and (answ2=1) then
          begin
            writeln(tx);
            writeln(tx,'Mutation');
          if (answ3=1) then
            begin
              writeln;
              writeln('Mutation');
            end;
          end;
      for k:=1 to qm do
        begin
          mx:=random(101);
          if (mx<=pm) then
            begin
              if (mode=3) and (answ2=1) then
                begin
                  writeln(tx,' Var: ',mx);
                  writeln(tx);
                  if (answ3=1) then
                    writeln(' Var: ',mx);
                end;
              mutation;
              if (mode=3) then
                writeln(tx);
              recount(p);
              sort;
            end
          else
            no('No mutation');
        end;
      sort;
      recount(p);
      if (p[1].res>rez) then
        rez:=p[1].res;
      if (mode=3) and (answ2=1) then
        begin
          writeln(tx);
          outputmastxt(p);
          if (answ3=1) then
            begin
              writeln;
              outputmas(p);
            end;
        end;
    end;
    
    {вывод}
    
  if (mode=2) or (mode=3) then
    begin
      writeln('Final set:');
      writeln;
      outputmas(p);
    end;
  if (mode=3) then 
    begin
      write(tx,'Final set:');
      writeln(tx);
      writeln(tx);
      outputmastxt(p);
      writeln(tx);
      write(tx,'Answer is: f(x)= ',rez:6:6,' with ',count,' iterations');
      close(tx);
    end;
  write('Answer is: f(x)= ',rez:6:6,' with ',count,' iterations');
end.
    
