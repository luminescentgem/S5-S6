import AbstractDAO      from "@/Asbtract/AbstractDAO.js";
import type DTO from "@/Interface/DTO.js";
import type Entity from "@/Interface/Entity.js";
import type Query from "@/Interface/Query.js";

export default class ArtistDAO extends AbstractDAO {
    exists(query: Query): Promise<boolean> {
        this.db.
    }

    find(query: Query): Promise<Entity> {
        
    }

    findAll(query: Query): Promise<Entity[]> {
        
    }

    update(dto: DTO): Promise<string> {
        
    }

    delete(query: Query): Promise<Entity[]> {
        
    }
}