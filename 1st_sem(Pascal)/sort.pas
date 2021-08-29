program prsort;
  const n=256;
  type vector = array[1..n] of integer;
  var x,y,z:vector;
      i,len,mode,step,inv,comp,k,d:integer;
  procedure trp(var p,q:integer); {замена элементов местами}
      var t:integer;
    begin
      t:=p;
      p:=q;
      q:=t;
    end;
  procedure shuttlesort(var inv,comp:integer); {челночная сортировка}
      var i,j,q:integer;
    begin
      for i:=2 to len do
      if d*x[i]<d*x[i-1] then {идем по массиву слева направо}
        begin
          comp:=comp+1;
          step:=step+1;
          if (mode=2) then {вывод для debug режима}
            begin
              writeln('Step: ',step);
              writeln('Comparsion elements: ',x[i-1],' ',x[i]); 
              for q:=1 to len do write(x[q],' ');
              writeln;
            end;
          for j:=i downto 2 do 
            {идем по массиву справа налево, чтобы найти  подходящее место
             для элемента}
            if d*x[j-1]>d*x[j] then
              begin  
                step:=step+1;
                inv:=inv+1;
                comp:=comp+1;
                trp(x[j],x[j-1]);
                if (mode=2) then {вывод для debug режима}
                  begin
                    writeln('Step: ',step);
                    writeln('Comparsion elements: ',x[i-1],' ',x[i]); 
                    for q:=1 to len do write(x[q],' ');
                    writeln;
                  end;
              end
            else 
              begin
                comp:=comp+1;
                step:=step+1;
                if (mode=2) then {вывод для debug режима}
                  begin
                    writeln('Step: ',step);
                    writeln('Comparsion elements: ',x[i-1],' ',x[i]); 
                    for q:=1 to len do write(x[q],' ');
                    writeln;
                  end;
              end;
        end
      else 
        begin
          step:=step+1;
          comp:=comp+1;
          begin
            if (mode=2) then {вывод для debug режима}
              begin
                writeln('Step: ',step);
                writeln('Comparsion elements: ',x[i-1],' ',x[i]);
                for q:=1 to len do write(x[q],' ');
                writeln;
              end;
          end;
        end;
    end;
   procedure mergesort(l,r:integer; var comp:integer);{сортировка присваиванием}
      var mid,i,j,n1,n2,q:integer;
    begin
      if l<r then {для сегментов, состоящих из более чем 1 символа}
        begin
          mid:=(l+r) div 2; {определение границ сегментов}
            {рекурсивые вызовы процедуры для работы с сегментами}
          mergesort(l,mid,comp); 
          mergesort(mid+1,r,comp);
          n1:=l;
          n2:=mid+1;
          step:=step+1;
          if (mode=2) then {вывод для debug режима}
            begin   
              writeln('Step: ',step);
              writeln('Comparsion elements: ',x[n1],' ',x[n2]);
            end;
          for i:=l to r do 
            begin
              if (n1<mid+1) and ((n2>r) or (d*x[n1]<d*x[n2])) then 
                {(n1<mid+1) and ((n2>r)...) на случай, если n2,n1 выходят за 
                границы сегмента}
                begin
                  y[i]:=x[n1]; {если х1<х2, переход к следующему}
                  n1:=n1+1;
                  comp:=comp+1;
                  inv:=inv+1;
                end
              else
                begin  
                  y[i]:=x[n2];
                  n2:=n2+1;
                  comp:=comp+1;
                  inv:=inv+1;
                end;
            end;
        if (mode=2) then {вывод для debug режима}
          begin
            for q:=1 to len do write(y[q],' ');
            writeln;
          end;
       for j:=l to r do
         x[j]:=y[j]; {изменение отсортированной части исходного массива}
     end;
   end;
begin
  for i:=1 to n do y[i]:=0;{обнуление для возможности вывода массива y}
  step:=0; 
  inv:=0;
  comp:=0;
  writeln('Enter sequence size');
  readln(len);
  if (len<0) then
    writeln('Wrong sequence size')
  else
    begin
      writeln('Enter sequence');
      for i:=1 to len do 
        begin
          read(x[i]);
          z[i]:=x[i];
           {массив z вводится т.к. после первой
            сортировки масив х уже отсортирован}
        end;
      writeln('Enter sort type (1-increasing sort(возрастание), 2-decreasing sort(убывание)');
      readln(k);{отвечает за вид сортировки: по возрастанию или по убыванию}
      if k=1 then d:=1;
      if k=2 then d:=-1;{-x<-y то же самое, что и x>y}
      writeln('Choose program mode (1 – Release, 2 - Debug)');
      readln(mode);
      writeln;{разделение между вводом и выводом}
      writeln('Source sequence: ');
      for i:=1 to len do write(x[i],' ');
      writeln;
      writeln('------------------------------');
        {для отделения введенной последовательности и результатов сортировки}
      if (mode<>1) and (mode<>2) then
          writeln('Wrong program mode');{на сучай неправильных mode,k}
      if (k<>1) and (k<>2) then
        writeln('Wrong sorting mode')
      else 
        begin
          if (mode=2) then writeln('Shuttle sort debug:')
          else writeln('Shuttle sort standart:');
          shuttlesort(inv,comp);
          writeln('Final sequence: ');
          for i:=1 to len do write(x[i],' ');
          writeln;
          writeln('Inversion count: ',inv);
          writeln('Comparsion count: ',comp);
          comp:=0;
          step:=0;
          writeln('------------------------------');
            {для различимой границы между двумя сортировками}
          for i:=1 to len do x[i]:=z[i];
          if(mode=2) then writeln('Merge sort debug:')
          else writeln('Merge sort standart:');
          inv:=0;
          mergesort(1,len,comp);
          writeln('Final sequence: ');
          for i:=1 to len do write(x[i],' ');
          writeln;
          writeln('Comparsion count: ',comp);
            {после каждого сравнения делается ровно одна запись в массив,
             поэтому колличество сравнений и перестановок(а так как 
             перестановок элементов массива нет, за перестановку считалась
             запись элемента x в y равно всегда}
          writeln('Inversion count: ',inv);
        end;
    end;
end.

        
