module Ruloir
  module Routing
    class Routes
      def get hsh
        path=hsh.keys.first
        template=hsh[path]
        
      end
    end
  end
end