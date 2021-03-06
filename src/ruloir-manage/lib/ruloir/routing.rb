module Ruloir
  module Routing
    class Routes
      def initialize
        @mapping={}
        @content_type="text/html"
      end
      
      def content_type(x)
        @content_type=x
      end
      
      def match method, hsh
        path=hsh.keys.first
        path=path.split('/')
        path.each(&:strip!)
        path.delete('')
        
        @mapping[method]||={}
        @mapping[method][path.join('/')]=hsh
      end
      
      def c_code
        code=""
      end
      
      private
      def encode(word, db, out)
        ch=word[0]
        db[ch]||={}
        if word.length==1
          db[ch][0]=out
        else
          encode(word[1..word.length-1],db[ch])
        end
      end
      def inner_string_select(db)
        code="switch(*#{str}){\n"
        db.each do |k,v|
          if k==0
            code<<"case 0: #{v} break;\n"
          else
            code<<"case '#{k}':\n++str;\n"
            code<<inner_string_select(db)
          end
        end
        code<<"case 0:\n" if db[0].nil?
        code<<"default: #{fail} break;\n}\n"
        code
      end
      def string_select(str,map,fail="return;")
        db={}
        map.keys.each { |x| encode(x, db, map[x]) }
        inner_string_select(db,fail)
      end
      def self.http_method(*meths)
        meths.each do |meth|
          send(:define_method,meth) do |hsh|
            match meth, hsh
          end
        end
      end
      public
      http_method :get, :post, :put, :patch, :head
    end
  end
end